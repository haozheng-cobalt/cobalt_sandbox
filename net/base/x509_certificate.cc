// Copyright 2008, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "net/base/x509_certificate.h"

#include <map>

#include "base/histogram.h"
#include "base/lock.h"
#include "base/pickle.h"
#include "base/singleton.h"
#include "base/string_tokenizer.h"
#include "base/string_util.h"
#include "net/base/cert_status_flags.h"
#include "net/base/ev_root_ca_metadata.h"

#pragma comment(lib, "crypt32.lib")

namespace {

// Returns true if this cert fingerprint is the null (all zero) fingerprint.
// We use this as a bogus fingerprint value.
bool IsNullFingerprint(const X509Certificate::Fingerprint& fingerprint) {
  for (int i = 0; i < arraysize(fingerprint.data); ++i) {
    if (fingerprint.data[i] != 0)
      return false;
  }
  return true;
}

// Calculates the SHA-1 fingerprint of the certificate.  Returns an empty
// (all zero) fingerprint on failure.
X509Certificate::Fingerprint CalculateFingerprint(PCCERT_CONTEXT cert) {
  DCHECK(NULL != cert->pbCertEncoded);
  DCHECK(0 != cert->cbCertEncoded);

  BOOL rv;
  X509Certificate::Fingerprint sha1;
  DWORD sha1_size = sizeof(sha1.data);
  rv = CryptHashCertificate(NULL, CALG_SHA1, 0, cert->pbCertEncoded,
                            cert->cbCertEncoded, sha1.data, &sha1_size);
  DCHECK(rv && sha1_size == sizeof(sha1.data));
  if (!rv)
    memset(sha1.data, 0, sizeof(sha1.data));
  return sha1;
}

// Wrappers of malloc and free for CRYPT_DECODE_PARA, which requires the
// WINAPI calling convention.
void* WINAPI MyCryptAlloc(size_t size) {
  return malloc(size);
}

void WINAPI MyCryptFree(void* p) {
  free(p);
}

// Decodes the cert's subjectAltName extension into a CERT_ALT_NAME_INFO
// structure and stores it in *output.
void GetCertSubjectAltName(PCCERT_CONTEXT cert,
                           scoped_ptr_malloc<CERT_ALT_NAME_INFO>* output) {
  PCERT_EXTENSION extension = CertFindExtension(szOID_SUBJECT_ALT_NAME2,
                                                cert->pCertInfo->cExtension,
                                                cert->pCertInfo->rgExtension);
  if (!extension)
    return;

  CRYPT_DECODE_PARA decode_para;
  decode_para.cbSize = sizeof(decode_para);
  decode_para.pfnAlloc = MyCryptAlloc;
  decode_para.pfnFree = MyCryptFree;
  CERT_ALT_NAME_INFO* alt_name_info = NULL;
  DWORD alt_name_info_size = 0;
  BOOL rv;
  rv = CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                           szOID_SUBJECT_ALT_NAME2,
                           extension->Value.pbData,
                           extension->Value.cbData,
                           CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                           &decode_para,
                           &alt_name_info,
                           &alt_name_info_size);
  if (rv)
    output->reset(alt_name_info);
}

///////////////////////////////////////////////////////////////////////////
//
// Functions used by X509Certificate::IsEV
//
///////////////////////////////////////////////////////////////////////////

// Constructs a certificate chain starting from the end certificate
// 'cert_context', matching any of the certificate policies.
//
// Returns the certificate chain context on success, or NULL on failure.
// The caller is responsible for freeing the certificate chain context with
// CertFreeCertificateChain.
PCCERT_CHAIN_CONTEXT ConstructCertChain(
    PCCERT_CONTEXT cert_context,
    const char* const* policies,
    int num_policies) {
  CERT_CHAIN_PARA chain_para;
  memset(&chain_para, 0, sizeof(chain_para));
  chain_para.cbSize = sizeof(chain_para);
  chain_para.RequestedUsage.dwType = USAGE_MATCH_TYPE_AND;
  chain_para.RequestedUsage.Usage.cUsageIdentifier = 0;
  chain_para.RequestedUsage.Usage.rgpszUsageIdentifier = NULL;  // LPSTR*
  chain_para.RequestedIssuancePolicy.dwType = USAGE_MATCH_TYPE_OR;
  chain_para.RequestedIssuancePolicy.Usage.cUsageIdentifier = num_policies;
  chain_para.RequestedIssuancePolicy.Usage.rgpszUsageIdentifier =
      const_cast<char**>(policies);
  PCCERT_CHAIN_CONTEXT chain_context;
  if (!CertGetCertificateChain(
      NULL,  // default chain engine, HCCE_CURRENT_USER
      cert_context,
      NULL,  // current system time
      cert_context->hCertStore,  // search this store
      &chain_para,
      CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT |
      CERT_CHAIN_CACHE_END_CERT,
      NULL,  // reserved
      &chain_context)) {
    return NULL;
  }
  return chain_context;
}

// Decodes the cert's certificatePolicies extension into a CERT_POLICIES_INFO
// structure and stores it in *output.
void GetCertPoliciesInfo(PCCERT_CONTEXT cert,
                         scoped_ptr_malloc<CERT_POLICIES_INFO>* output) {
  PCERT_EXTENSION extension = CertFindExtension(szOID_CERT_POLICIES,
                                                cert->pCertInfo->cExtension,
                                                cert->pCertInfo->rgExtension);
  if (!extension)
    return;

  CRYPT_DECODE_PARA decode_para;
  decode_para.cbSize = sizeof(decode_para);
  decode_para.pfnAlloc = MyCryptAlloc;
  decode_para.pfnFree = MyCryptFree;
  CERT_POLICIES_INFO* policies_info = NULL;
  DWORD policies_info_size = 0;
  BOOL rv;
  rv = CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                           szOID_CERT_POLICIES,
                           extension->Value.pbData,
                           extension->Value.cbData,
                           CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                           &decode_para,
                           &policies_info,
                           &policies_info_size);
  if (rv)
    output->reset(policies_info);
}

// Returns true if the policy is in the array of CERT_POLICY_INFO in
// the CERT_POLICIES_INFO structure.
bool ContainsPolicy(const CERT_POLICIES_INFO* policies_info,
                    const char* policy) {
  int num_policies = policies_info->cPolicyInfo;
  for (int i = 0; i < num_policies; i++) {
    if (!strcmp(policies_info->rgPolicyInfo[i].pszPolicyIdentifier, policy))
      return true;
  }
  return false;
}

// This class wraps the CertFreeCertificateChain function in a class that can
// be passed as a template argument to scoped_ptr_malloc.
class ScopedPtrMallocFreeCertChain {
 public:
  void operator()(const CERT_CHAIN_CONTEXT* x) const {
    CertFreeCertificateChain(x);
  }
};

typedef scoped_ptr_malloc<const CERT_CHAIN_CONTEXT,
                          ScopedPtrMallocFreeCertChain> ScopedCertChainContext;

}  // namespace

bool X509Certificate::FingerprintLessThan::operator()(
    const Fingerprint& lhs,
    const Fingerprint& rhs) const {
  for (int i = 0; i < sizeof(lhs.data); ++i) {
    if (lhs.data[i] < rhs.data[i])
      return true;
    if (lhs.data[i] > rhs.data[i])
      return false;
  }
  return false;
}

bool X509Certificate::LessThan::operator()(X509Certificate* lhs,
                                           X509Certificate* rhs) const {
  if (lhs == rhs)
    return false;

  X509Certificate::FingerprintLessThan fingerprint_functor;
  return fingerprint_functor(lhs->fingerprint_, rhs->fingerprint_);
}

// A thread-safe cache for X509Certificate objects.
//
// The cache does not hold a reference to the certificate objects.  The objects
// must |Remove| themselves from the cache upon destruction (or else the cache
// will be holding dead pointers to the objects).
class X509Certificate::Cache {
 public:
  // Get the singleton object for the cache.
  static X509Certificate::Cache* GetInstance() {
    return Singleton<X509Certificate::Cache>::get();
  }

  // Insert |cert| into the cache.  The cache does NOT AddRef |cert|.  The cache
  // must not already contain a certificate with the same fingerprint.
  void Insert(X509Certificate* cert) {
    AutoLock lock(lock_);

    DCHECK(!IsNullFingerprint(cert->fingerprint())) <<
        "Only insert certs with real fingerprints.";
    DCHECK(cache_.find(cert->fingerprint()) == cache_.end());
    cache_[cert->fingerprint()] = cert;
  };

  // Remove |cert| from the cache.  The cache does not assume that |cert| is
  // already in the cache.
  void Remove(X509Certificate* cert) {
    AutoLock lock(lock_);

    CertMap::iterator pos(cache_.find(cert->fingerprint()));
    if (pos == cache_.end())
      return;  // It is not an error to remove a cert that is not in the cache.
    cache_.erase(pos);
  };

  // Find a certificate in the cache with the given fingerprint.  If one does
  // not exist, this method returns NULL.
  X509Certificate* Find(const Fingerprint& fingerprint) {
    AutoLock lock(lock_);

    CertMap::iterator pos(cache_.find(fingerprint));
    if (pos == cache_.end())
      return NULL;

    return pos->second;
  };

 private:
  typedef std::map<Fingerprint, X509Certificate*, FingerprintLessThan> CertMap;

  // Obtain an instance of X509Certificate::Cache via GetInstance().
  Cache() { }
  friend DefaultSingletonTraits<X509Certificate::Cache>;

  // You must acquire this lock before using any private data of this object.
  // You must not block while holding this lock.
  Lock lock_;

  // The certificate cache.  You must acquire |lock_| before using |cache_|.
  CertMap cache_;

  DISALLOW_EVIL_CONSTRUCTORS(X509Certificate::Cache);
};

void X509Certificate::Initialize() {
  std::wstring subject_info;
  std::wstring issuer_info;
  DWORD name_size;
  name_size = CertNameToStr(cert_handle_->dwCertEncodingType,
                            &cert_handle_->pCertInfo->Subject,
                            CERT_X500_NAME_STR | CERT_NAME_STR_CRLF_FLAG,
                            NULL, 0);
  name_size = CertNameToStr(cert_handle_->dwCertEncodingType,
                            &cert_handle_->pCertInfo->Subject,
                            CERT_X500_NAME_STR | CERT_NAME_STR_CRLF_FLAG,
                            WriteInto(&subject_info, name_size), name_size);
  name_size = CertNameToStr(cert_handle_->dwCertEncodingType,
                            &cert_handle_->pCertInfo->Issuer,
                            CERT_X500_NAME_STR | CERT_NAME_STR_CRLF_FLAG,
                            NULL, 0);
  name_size = CertNameToStr(cert_handle_->dwCertEncodingType,
                            &cert_handle_->pCertInfo->Issuer,
                            CERT_X500_NAME_STR | CERT_NAME_STR_CRLF_FLAG,
                            WriteInto(&issuer_info, name_size), name_size);
  ParsePrincipal(WideToUTF8(subject_info), &subject_);
  ParsePrincipal(WideToUTF8(issuer_info), &issuer_);

  valid_start_ = Time::FromFileTime(cert_handle_->pCertInfo->NotBefore);
  valid_expiry_ = Time::FromFileTime(cert_handle_->pCertInfo->NotAfter);

  fingerprint_ = CalculateFingerprint(cert_handle_);

  // Store the certificate in the cache in case we need it later.
  X509Certificate::Cache::GetInstance()->Insert(this);
}

// static
X509Certificate* X509Certificate::CreateFromHandle(OSCertHandle cert_handle) {
  DCHECK(cert_handle);

  // Check if we already have this certificate in memory.
  X509Certificate::Cache* cache = X509Certificate::Cache::GetInstance();
  X509Certificate* cert = cache->Find(CalculateFingerprint(cert_handle));
  if (cert) {
    // We've found a certificate with the same fingerprint in our cache.  We own
    // the |cert_handle|, which makes it our job to free it.
    CertFreeCertificateContext(cert_handle);
    DHISTOGRAM_COUNTS(L"X509CertificateReuseCount", 1);
    return cert;
  }
  // Otherwise, allocate a new object.
  return new X509Certificate(cert_handle);
}

// static
X509Certificate* X509Certificate::CreateFromPickle(const Pickle& pickle,
                                                   void** pickle_iter) {
  const char* data;
  int length;
  if (!pickle.ReadData(pickle_iter, &data, &length))
    return NULL;

  OSCertHandle cert_handle = NULL;
  if (!CertAddSerializedElementToStore(
      NULL,  // the cert won't be persisted in any cert store
      reinterpret_cast<const BYTE*>(data), length,
      CERT_STORE_ADD_USE_EXISTING, 0, CERT_STORE_CERTIFICATE_CONTEXT_FLAG,
      NULL, reinterpret_cast<const void **>(&cert_handle)))
    return NULL;

  return CreateFromHandle(cert_handle);
}

X509Certificate::X509Certificate(OSCertHandle cert_handle)
    : cert_handle_(cert_handle) {
  Initialize();
}

X509Certificate::X509Certificate(std::string subject, std::string issuer,
                                 Time start_date, Time expiration_date)
    : subject_(subject),
      issuer_(issuer),
      valid_start_(start_date),
      valid_expiry_(expiration_date),
      cert_handle_(NULL) {
  memset(fingerprint_.data, 0, sizeof(fingerprint_.data));
}

void X509Certificate::Persist(Pickle* pickle) {
  DWORD length;
  if (!CertSerializeCertificateStoreElement(cert_handle_, 0,
      NULL, &length)) {
    NOTREACHED();
    return;
  }
  BYTE* data = reinterpret_cast<BYTE*>(pickle->BeginWriteData(length));
  if (!CertSerializeCertificateStoreElement(cert_handle_, 0,
      data, &length)) {
    NOTREACHED();
    length = 0;
  }
  pickle->TrimWriteData(length);
}

X509Certificate::~X509Certificate() {
  // We might not be in the cache, but it is safe to remove ourselves anyway.
  X509Certificate::Cache::GetInstance()->Remove(this);
  if (cert_handle_)
    CertFreeCertificateContext(cert_handle_);
}

void X509Certificate::GetDNSNames(std::vector<std::string>* dns_names) const {
  dns_names->clear();
  scoped_ptr_malloc<CERT_ALT_NAME_INFO> alt_name_info;
  GetCertSubjectAltName(cert_handle_, &alt_name_info);
  CERT_ALT_NAME_INFO* alt_name = alt_name_info.get();
  if (alt_name) {
    int num_entries = alt_name->cAltEntry;
    for (int i = 0; i < num_entries; i++) {
      // dNSName is an ASN.1 IA5String representing a string of ASCII
      // characters, so we can use WideToASCII here.
      if (alt_name->rgAltEntry[i].dwAltNameChoice == CERT_ALT_NAME_DNS_NAME)
        dns_names->push_back(WideToASCII(alt_name->rgAltEntry[i].pwszDNSName));
    }
  }
  if (dns_names->empty())
    dns_names->push_back(subject_.common_name);
}

bool X509Certificate::HasExpired() const {
  return Time::Now() > valid_expiry();
}

// Returns true if the certificate is an extended-validation certificate.
//
// The certificate has already been verified by the HTTP library.  cert_status
// represents the result of that verification.  This function performs
// additional checks of the certificatePolicies extensions of the certificates
// in the certificate chain according to Section 7 (pp. 11-12) of the EV
// Certificate Guidelines Version 1.0 at
// http://cabforum.org/EV_Certificate_Guidelines.pdf.
bool X509Certificate::IsEV(int cert_status) const {
  if (net::IsCertStatusError(cert_status) ||
      (cert_status & net::CERT_STATUS_REV_CHECKING_ENABLED) == 0)
    return false;

  net::EVRootCAMetadata* metadata = net::EVRootCAMetadata::GetInstance();

  PCCERT_CHAIN_CONTEXT chain_context = ConstructCertChain(cert_handle_,
      metadata->GetPolicyOIDs(), metadata->NumPolicyOIDs());
  if (!chain_context)
    return false;
  ScopedCertChainContext scoped_chain_context(chain_context);

  DCHECK(chain_context->cChain != 0);
  // If the cert doesn't match any of the policies, the
  // CERT_TRUST_IS_NOT_VALID_FOR_USAGE bit (0x10) in
  // chain_context->TrustStatus.dwErrorStatus is set.
  DWORD error_status = chain_context->TrustStatus.dwErrorStatus;
  DWORD info_status = chain_context->TrustStatus.dwInfoStatus;
  if (!chain_context->cChain || error_status != CERT_TRUST_NO_ERROR)
    return false;

  // Check the end certificate simple chain (chain_context->rgpChain[0]).
  // If the end certificate's certificatePolicies extension contains the
  // EV policy OID of the root CA, return true.
  PCERT_CHAIN_ELEMENT* element = chain_context->rgpChain[0]->rgpElement;
  int num_elements = chain_context->rgpChain[0]->cElement;
  if (num_elements < 2)
    return false;

  // Look up the EV policy OID of the root CA.
  PCCERT_CONTEXT root_cert = element[num_elements - 1]->pCertContext;
  X509Certificate::Fingerprint fingerprint = CalculateFingerprint(root_cert);
  std::string ev_policy_oid;
  if (!metadata->GetPolicyOID(fingerprint, &ev_policy_oid))
    return false;
  DCHECK(!ev_policy_oid.empty());

  // Get the certificatePolicies extension of the end certificate.
  PCCERT_CONTEXT end_cert = element[0]->pCertContext;
  scoped_ptr_malloc<CERT_POLICIES_INFO> policies_info;
  GetCertPoliciesInfo(end_cert, &policies_info);
  if (!policies_info.get())
    return false;

  return ContainsPolicy(policies_info.get(), ev_policy_oid.c_str());
}

// static
void X509Certificate::ParsePrincipal(const std::string& description,
                                     Principal* principal) {
  // The description of the principal is a string with each LDAP value on
  // a separate line.
  const std::string kDelimiters("\r\n");

  std::vector<std::string> common_names, locality_names, state_names,
      country_names, street_addresses;

  // TODO(jcampan): add business_category and serial_number.
  const std::string kPrefixes[8] = { std::string("CN="),
                                     std::string("L="),
                                     std::string("S="),
                                     std::string("C="),
                                     std::string("STREET="),
                                     std::string("O="),
                                     std::string("OU="),
                                     std::string("DC=") };

  std::vector<std::string>* values[8] = {
      &common_names, &locality_names,
      &state_names, &country_names,
      &(principal->street_addresses),
      &(principal->organization_names),
      &(principal->organization_unit_names),
      &(principal->domain_components) };
  DCHECK(arraysize(kPrefixes) == arraysize(values));

  StringTokenizer str_tok(description, kDelimiters);
  while (str_tok.GetNext()) {
    std::string entry = str_tok.token();
    for (int i = 0; i < arraysize(kPrefixes); i++) {
      if (!entry.compare(0, kPrefixes[i].length(), kPrefixes[i])) {
        std::string value = entry.substr(kPrefixes[i].length());
        // Remove enclosing double-quotes if any.
        if (value.size() >= 2 &&
            value[0] == '"' && value[value.size() - 1] == '"')
          value = value.substr(1, value.size() - 2);
        values[i]->push_back(value);
        break;
      }
    }
  }

  // We don't expect to have more than one CN, L, S, and C.
  std::vector<std::string>* single_value_lists[4] = {
      &common_names, &locality_names, &state_names, &country_names };
  std::string* single_values[4] = {
      &principal->common_name, &principal->locality_name,
      &principal->state_or_province_name, &principal->country_name };
  for (int i = 0; i < arraysize(single_value_lists); ++i) {
    int length = static_cast<int>(single_value_lists[i]->size());
    DCHECK(single_value_lists[i]->size() <= 1);
    if (single_value_lists[i]->size() > 0)
      *(single_values[i]) = (*(single_value_lists[i]))[0];
  }
}

X509Certificate::Policy::Judgment X509Certificate::Policy::Check(
    X509Certificate* cert) const {
  // It shouldn't matter which set we check first, but we check denied first
  // in case something strange has happened.

  if (denied_.find(cert->fingerprint()) != denied_.end()) {
    // DCHECK that the order didn't matter.
    DCHECK(allowed_.find(cert->fingerprint()) == allowed_.end());
    return DENIED;
  }

  if (allowed_.find(cert->fingerprint()) != allowed_.end()) {
    // DCHECK that the order didn't matter.
    DCHECK(denied_.find(cert->fingerprint()) == denied_.end());
    return ALLOWED;
  }

  // We don't have a policy for this cert.
  return UNKNOWN;
}

void X509Certificate::Policy::Allow(X509Certificate* cert) {
  // Put the cert in the allowed set and (maybe) remove it from the denied set.
  denied_.erase(cert->fingerprint());
  allowed_.insert(cert->fingerprint());
}

void X509Certificate::Policy::Deny(X509Certificate* cert) {
  // Put the cert in the denied set and (maybe) remove it from the allowed set.
  allowed_.erase(cert->fingerprint());
  denied_.insert(cert->fingerprint());
}

/*
 * Copyright 2015 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cobalt/loader/file_fetcher.h"

#include "base/bind.h"
#include "base/file_util.h"
#include "base/file_util_proxy.h"
#include "base/path_service.h"

namespace cobalt {
namespace loader {

FileFetcher::FileFetcher(const FilePath& file_path, Handler* handler,
                         const Options& options)
    : Fetcher(handler),
      buffer_size_(options.buffer_size),
      file_(base::kInvalidPlatformFileValue),
      file_offset_(options.start_offset),
      bytes_left_to_read_(options.bytes_to_read),
      message_loop_proxy_(options.message_loop_proxy),
      ALLOW_THIS_IN_INITIALIZER_LIST(weak_ptr_factory_(this)) {
  DCHECK_GT(buffer_size_, 0);

  // Get the actual file path by prepending DIR_SOURCE_ROOT to the file path.
  FilePath actual_file_path;
  PathService::Get(base::DIR_SOURCE_ROOT, &actual_file_path);
  actual_file_path = actual_file_path.Append(file_path);

  // Trigger fetching in the given message loop.
  base::FileUtilProxy::CreateOrOpen(
      message_loop_proxy_, actual_file_path,
      base::PLATFORM_FILE_OPEN | base::PLATFORM_FILE_READ,
      base::Bind(&FileFetcher::DidCreateOrOpen,
                 weak_ptr_factory_.GetWeakPtr()));
}

FileFetcher::~FileFetcher() {
  DCHECK(thread_checker_.CalledOnValidThread());
  CloseFile();
}

void FileFetcher::ReadNextChunk() {
  int32 bytes_to_read = buffer_size_;
  if (bytes_to_read > bytes_left_to_read_) {
    bytes_to_read = static_cast<int32>(bytes_left_to_read_);
  }
  base::FileUtilProxy::Read(
      message_loop_proxy_, file_, file_offset_, bytes_to_read,
      base::Bind(&FileFetcher::DidRead, weak_ptr_factory_.GetWeakPtr()));
}

void FileFetcher::CloseFile() {
  if (file_ != base::kInvalidPlatformFileValue) {
    base::ClosePlatformFile(file_);
    file_ = base::kInvalidPlatformFileValue;
  }
}

const char* FileFetcher::PlatformFileErrorToString(
    base::PlatformFileError error) {
  static const char kPlatformFileOk[] = "Platform file OK";
  static const char kPlatformFileErrorNotFound[] =
      "Platform file error: Not found";
  static const char kPlatformFileErrorInUse[] = "Platform file error: In use";
  static const char kPlatformFileErrorAccessDenied[] =
      "Platform file error: Access denied";
  static const char kPlatformFileErrorSecurity[] =
      "Platform file error: Security";
  static const char kPlatformFileErrorInvalidUrl[] =
      "Platform file error: Invalid URL";
  static const char kPlatformFileErrorAbort[] = "Platform file error: Abort";
  static const char kPlatformFileErrorNotAFile[] =
      "Platform file error: Not a file";
  static const char kPlatformFileErrorNotDefined[] =
      "Platform file error: Undefined error";

  switch (error) {
    case base::PLATFORM_FILE_OK:
      return kPlatformFileOk;
    case base::PLATFORM_FILE_ERROR_NOT_FOUND:
      return kPlatformFileErrorNotFound;
    case base::PLATFORM_FILE_ERROR_IN_USE:
      return kPlatformFileErrorInUse;
    case base::PLATFORM_FILE_ERROR_ACCESS_DENIED:
      return kPlatformFileErrorAccessDenied;
    case base::PLATFORM_FILE_ERROR_SECURITY:
      return kPlatformFileErrorSecurity;
    case base::PLATFORM_FILE_ERROR_INVALID_URL:
      return kPlatformFileErrorInvalidUrl;
    case base::PLATFORM_FILE_ERROR_ABORT:
      return kPlatformFileErrorAbort;
    case base::PLATFORM_FILE_ERROR_NOT_A_FILE:
      return kPlatformFileErrorNotAFile;
    case base::PLATFORM_FILE_ERROR_FAILED:
    case base::PLATFORM_FILE_ERROR_EXISTS:
    case base::PLATFORM_FILE_ERROR_TOO_MANY_OPENED:
    case base::PLATFORM_FILE_ERROR_NO_MEMORY:
    case base::PLATFORM_FILE_ERROR_NO_SPACE:
    case base::PLATFORM_FILE_ERROR_NOT_A_DIRECTORY:
    case base::PLATFORM_FILE_ERROR_INVALID_OPERATION:
    case base::PLATFORM_FILE_ERROR_NOT_EMPTY:
    case base::PLATFORM_FILE_ERROR_MAX:
    default:
      break;
  }
  return kPlatformFileErrorNotDefined;
}

void FileFetcher::DidCreateOrOpen(base::PlatformFileError error,
                                  base::PassPlatformFile file,
                                  bool /*created*/) {
  DCHECK(thread_checker_.CalledOnValidThread());
  if (error != base::PLATFORM_FILE_OK) {
    handler()->OnError(PlatformFileErrorToString(error));
    return;
  }

  file_ = file.ReleaseValue();
  ReadNextChunk();
}

void FileFetcher::DidRead(base::PlatformFileError error, const char* data,
                          int num_bytes_read) {
  DCHECK(thread_checker_.CalledOnValidThread());

  if (error != base::PLATFORM_FILE_OK) {
    handler()->OnError(PlatformFileErrorToString(error));
    return;
  }

  DCHECK_LE(num_bytes_read, bytes_left_to_read_);

  if (!num_bytes_read) {
    handler()->OnDone();
    return;
  }

  handler()->OnReceived(data, static_cast<size_t>(num_bytes_read));

  bytes_left_to_read_ -= num_bytes_read;

  if (!bytes_left_to_read_) {
    handler()->OnDone();
    return;
  }

  file_offset_ += num_bytes_read;
  ReadNextChunk();
}

}  // namespace loader
}  // namespace cobalt

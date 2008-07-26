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

// This is an internal class that handles the address of a cache record.
// See net/disk_cache/disk_cache.h for the public interface of the cache.

#ifndef NET_DISK_CACHE_ADDR_H__
#define NET_DISK_CACHE_ADDR_H__

#include "base/basictypes.h"
#include "base/logging.h"
#include "net/disk_cache/disk_format.h"

namespace disk_cache {

enum FileType {
  EXTERNAL = 0,
  RANKINGS = 1,
  BLOCK_256,
  BLOCK_1K,
  BLOCK_4K,
};

const int kMaxBlockSize = 4096 * 4;
const int kMaxBlockFile = 255;
const int kMaxNumBlocks = 4;
const int kFirstAdditionlBlockFile = 4;

// Defines a storage address for a cache record
//
// Header:
//   1000 0000 0000 0000 0000 0000 0000 0000 : initialized bit
//   0111 0000 0000 0000 0000 0000 0000 0000 : file type
//
// File type values:
//   0 = separate file on disk
//   1 = rankings block file
//   2 = 256 byte block file
//   3 = 1k byte block file
//   4 = 4k byte block file
//
// If separate file:
//   0000 1111 1111 1111 1111 1111 1111 1111 : file#  0 - 268,435,456 (2^28)
//
// If block file:
//   0000 1100 0000 0000 0000 0000 0000 0000 : reserved bits
//   0000 0011 0000 0000 0000 0000 0000 0000 : number of contiguous blocks 1-4
//   0000 0000 1111 1111 0000 0000 0000 0000 : file selector 0 - 255
//   0000 0000 0000 0000 1111 1111 1111 1111 : block#  0 - 65,535 (2^16)
class Addr {
 public:
  explicit Addr(CacheAddr address) : value_(address) {}
  Addr(FileType file_type, int max_blocks, int block_file, int index) {
    value_ = ((file_type << kFileTypeOffset) & kFileTypeMask) |
             (((max_blocks - 1) << kNumBlocksOffset) & kNumBlocksMask) |
             ((block_file << kFileSelectorOffset) & kFileSelectorMask) |
             (index  & kStartBlockMask) | kInitializedMask;
  }

  CacheAddr value() const { return value_; }
  void set_value(CacheAddr address) {
    value_ = address;
  }

  bool is_initialized() const {
    return (value_ & kInitializedMask) != 0;
  }

  bool is_separate_file() const {
    return (value_ & kFileTypeMask) == 0;
  }

  bool is_block_file() const {
    return !is_separate_file();
  }

  FileType file_type() const {
    return static_cast<FileType>((value_ & kFileTypeMask) >> kFileTypeOffset);
  }

  int FileNumber() const {
    if (is_separate_file())
      return value_ & kFileNameMask;
    else
      return ((value_ & kFileSelectorMask) >> kFileSelectorOffset);
  }

  int start_block() const {
    DCHECK(is_block_file());
    return value_ & kStartBlockMask;
  }

  int num_blocks() const {
    DCHECK(is_block_file() || !value_);
    return ((value_ & kNumBlocksMask) >> kNumBlocksOffset) + 1;
  }

  bool SetFileNumber(int file_number) {
    DCHECK(is_separate_file());
    if (file_number & ~kFileNameMask)
      return false;
    value_ = kInitializedMask | file_number;
    return true;
  }

  int BlockSize() const {
    return BlockSizeForFileType(file_type());
  }

  static int BlockSizeForFileType(FileType file_type) {
    switch (file_type) {
      case RANKINGS:
        return 36;
      case BLOCK_256:
        return 256;
      case BLOCK_1K:
        return 1024;
      case BLOCK_4K:
        return 4096;
      default:
        return 0;
    }
  }

  static FileType RequiredFileType(int size) {
    if (size < 1024)
      return BLOCK_256;
    else if (size < 4096)
      return BLOCK_1K;
    else if (size <= 4096 * 4)
      return BLOCK_4K;
    else
      return EXTERNAL;
  }

 private:
  static const uint32 kInitializedMask    = 0x80000000;
  static const uint32 kFileTypeMask       = 0x70000000;
  static const uint32 kFileTypeOffset     = 28;
  static const uint32 kNumBlocksMask      = 0x03000000;
  static const uint32 kNumBlocksOffset    = 24;
  static const uint32 kFileSelectorMask   = 0x00ff0000;
  static const uint32 kFileSelectorOffset = 16;
  static const uint32 kStartBlockMask     = 0x0000FFFF;
  static const uint32 kFileNameMask       = 0x0FFFFFFF;

  CacheAddr value_;
};

}  // namespace disk_cache

#endif  // NET_DISK_CACHE_ADDR_H__

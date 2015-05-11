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

#ifndef CSSOM_SCOPED_LIST_VALUE_H_
#define CSSOM_SCOPED_LIST_VALUE_H_

#include <vector>

#include "base/compiler_specific.h"
#include "base/memory/scoped_vector.h"
#include "cobalt/cssom/property_value.h"
#include "cobalt/cssom/property_value_visitor.h"

namespace cobalt {
namespace cssom {

// This class can be used to represent a CSS property value that is a list of
// heap-allocated polymorphic objects.  If the list elements do not need to
// be allocated on the heap, then it may be more efficient to use ListValue
// instead.  All elements must derive from base::PolymorphicEquatable so that
// they can be tested for equality by value.
template <typename T>
class ScopedListValue : public PropertyValue {
 public:
  typedef ScopedVector<T> Builder;

  explicit ScopedListValue(Builder value) : value_(value.Pass()) {}

  const Builder& value() const { return value_; }

  bool operator==(const ScopedListValue<T>& other) const {
    if (value_.size() != other.value_.size()) return false;

    typename Builder::const_iterator iter_a = value_.begin();
    typename Builder::const_iterator iter_b = other.value_.begin();
    for (; iter_a != value_.end(); ++iter_a, ++iter_b) {
      if (!(*iter_a)->Equals(**iter_b)) {
        return false;
      }
    }

    return true;
  }

 protected:
  ~ScopedListValue() OVERRIDE {}

  const Builder value_;

 private:
  DISALLOW_COPY_AND_ASSIGN(ScopedListValue<T>);
};

}  // namespace cssom
}  // namespace cobalt

#endif  // CSSOM_SCOPED_LIST_VALUE_H_

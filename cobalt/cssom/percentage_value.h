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

#ifndef CSSOM_PERCENTAGE_VALUE_H_
#define CSSOM_PERCENTAGE_VALUE_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "cobalt/cssom/property_value.h"

namespace cobalt {
namespace cssom {

// Percentage values are always relative to another value, for example a length.
// Each property that allows percentages also defines the value to which
// the percentage refers.
//   http://www.w3.org/TR/css3-values/#percentages
class PercentageValue : public PropertyValue {
 public:
  // A |value| is a normalized factor, where 1 means 100%.
  explicit PercentageValue(float value) : value_(value) {}

  virtual void Accept(PropertyValueVisitor* visitor) OVERRIDE;

  // Returns a normalized factor, where 1 means 100%.
  float value() const { return value_; }

  bool operator==(const PercentageValue& other) const {
    return value_ == other.value_;
  }

  DEFINE_POLYMORPHIC_EQUATABLE_TYPE(PercentageValue);

 private:
  ~PercentageValue() OVERRIDE {}

  const float value_;

  DISALLOW_COPY_AND_ASSIGN(PercentageValue);
};

}  // namespace cssom
}  // namespace cobalt

#endif  // CSSOM_PERCENTAGE_VALUE_H_

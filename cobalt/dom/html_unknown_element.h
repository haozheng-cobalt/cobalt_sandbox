/*
 * Copyright 2014 Google Inc. All Rights Reserved.
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

#ifndef DOM_HTML_UNKNOWN_ELEMENT_H_
#define DOM_HTML_UNKNOWN_ELEMENT_H_

#include <string>

#include "base/string_piece.h"
#include "cobalt/dom/html_element.h"

namespace cobalt {
namespace dom {

// The HTMLUnknownElement interface must be used for HTML elements that are not
// defined by any applicable specifications.
//   http://www.w3.org/TR/html5/dom.html#htmlunknownelement
class HTMLUnknownElement : public HTMLElement {
 public:
  HTMLUnknownElement(Document* document,
                     HTMLElementContext* html_element_context,
                     const base::StringPiece& tag_name);

  // Web API: Element
  std::string tag_name() const OVERRIDE;

  DEFINE_WRAPPABLE_TYPE(HTMLUnknownElement);

 private:
  ~HTMLUnknownElement() OVERRIDE;

  std::string tag_name_;
};

}  // namespace dom
}  // namespace cobalt

#endif  // DOM_HTML_UNKNOWN_ELEMENT_H_

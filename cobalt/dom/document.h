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

#ifndef COBALT_DOM_DOCUMENT_H_
#define COBALT_DOM_DOCUMENT_H_

#include <deque>
#include <map>
#include <string>

#include "base/callback.h"
#include "base/memory/weak_ptr.h"
#include "base/observer_list.h"
#include "base/optional.h"
#include "base/string_piece.h"
#include "cobalt/base/clock.h"
#include "cobalt/cssom/css_keyframes_rule.h"
#include "cobalt/cssom/css_style_sheet.h"
#include "cobalt/cssom/mutation_observer.h"
#include "cobalt/cssom/selector_tree.h"
#include "cobalt/cssom/style_sheet_list.h"
#include "cobalt/dom/document_timeline.h"
#include "cobalt/dom/event.h"
#include "cobalt/dom/location.h"
#include "cobalt/dom/node.h"
#include "cobalt/math/size.h"
#include "cobalt/network_bridge/cookie_jar.h"
#include "cobalt/network_bridge/net_poster.h"
#include "cobalt/script/exception_state.h"
#include "googleurl/src/gurl.h"

namespace cobalt {
namespace dom {

class Attr;
class CSPDelegate;
class DOMImplementation;
class Element;
class FontCache;
class HTMLBodyElement;
class HTMLCollection;
class HTMLElement;
class HTMLElementContext;
class HTMLHeadElement;
class HTMLHtmlElement;
class HTMLScriptElement;
class Location;
class NodeList;
class Text;

class DocumentObserver {
 public:
  // Called at most once, when document and all referred resources are loaded.
  virtual void OnLoad() = 0;

  // Called each time when the document or one of its descendants is changed.
  virtual void OnMutation() = 0;

 protected:
  virtual ~DocumentObserver() {}
};

// The Document interface serves as an entry point into the web page's content
// (the DOM tree, including elements such as <head> and <body>) and provides
// functionality which is global to the document.
//   http://www.w3.org/TR/2014/WD-dom-20140710/#interface-document
//
// In the spec, "A document is assumed to be an XML document unless it is
// flagged as being an HTML document". In Cobalt it is always considered as HTML
// document.
class Document : public Node, public cssom::MutationObserver {
 public:
  struct Options {
    Options() : cookie_jar(NULL) {}
    explicit Options(const GURL& url_value)
        : url(url_value), cookie_jar(NULL) {}
    Options(const GURL& url_value,
            const scoped_refptr<base::Clock>& navigation_start_clock_value,
            const base::Callback<void(const GURL&)>& navigation_callback,
            const scoped_refptr<cssom::CSSStyleSheet> user_agent_style_sheet,
            const base::optional<math::Size>& viewport_size,
            network_bridge::CookieJar* cookie_jar,
            const network_bridge::NetPosterFactory& net_poster_factory)
        : url(url_value),
          navigation_start_clock(navigation_start_clock_value),
          navigation_callback(navigation_callback),
          user_agent_style_sheet(user_agent_style_sheet),
          viewport_size(viewport_size),
          cookie_jar(cookie_jar),
          net_poster_factory(net_poster_factory) {}

    GURL url;
    scoped_refptr<base::Clock> navigation_start_clock;
    base::Callback<void(const GURL&)> navigation_callback;
    scoped_refptr<cssom::CSSStyleSheet> user_agent_style_sheet;
    base::optional<math::Size> viewport_size;
    network_bridge::CookieJar* cookie_jar;
    network_bridge::NetPosterFactory net_poster_factory;
  };

  Document(HTMLElementContext* html_element_context,
           const Options& options = Options());

  // Web API: Node
  //
  NodeType node_type() const OVERRIDE { return Node::kDocumentNode; }
  std::string node_name() const OVERRIDE;

  // Web API: Document
  //
  scoped_refptr<DOMImplementation> implementation();
  const std::string& url() const { return location_->url().spec(); }
  const std::string& document_uri() const { return location_->url().spec(); }

  scoped_refptr<Element> document_element();
  std::string title() const;

  scoped_refptr<HTMLCollection> GetElementsByTagName(
      const std::string& local_name) const;
  scoped_refptr<HTMLCollection> GetElementsByClassName(
      const std::string& class_names) const;

  scoped_refptr<Element> CreateElement(const std::string& local_name);
  scoped_refptr<Element> CreateElementNS(const std::string& namespace_uri,
                                         const std::string& local_name);
  scoped_refptr<Text> CreateTextNode(const std::string& data);
  scoped_refptr<Event> CreateEvent(const std::string& interface_name,
                                   script::ExceptionState* exception_state);

  // Web API: NonElementParentNode (implements)
  //   http://www.w3.org/TR/2014/WD-dom-20140710/#interface-nonelementparentnode
  //
  scoped_refptr<Element> GetElementById(const std::string& id) const;

  // Web API: HTML5 (partial interface)
  //   http://www.w3.org/TR/html5/dom.html#the-document-object
  //
  scoped_refptr<Location> location() const;

  scoped_refptr<HTMLBodyElement> body() const;
  void set_body(const scoped_refptr<HTMLBodyElement>& value);

  scoped_refptr<HTMLHeadElement> head() const;

  scoped_refptr<Element> active_element() const;

  // Web API: CSS Object Model (partial interface)
  //   http://dev.w3.org/csswg/cssom/#extensions-to-the-document-interface
  const scoped_refptr<cssom::StyleSheetList>& style_sheets() const {
    return style_sheets_;
  }

  // Web API: Selectors API (partial interface)
  //   http://www.w3.org/TR/selectors-api2/#interface-definitions
  //
  scoped_refptr<Element> QuerySelector(const std::string& selectors);
  scoped_refptr<NodeList> QuerySelectorAll(const std::string& selectors);

  // Web Animations API
  // http://www.w3.org/TR/2015/WD-web-animations-1-20150707/#extensions-to-the-document-interface
  const scoped_refptr<DocumentTimeline>& timeline() const {
    return default_timeline_;
  }

  // http://www.w3.org/TR/html5/dom.html#dom-document-cookie
  void set_cookie(const std::string& cookie);
  std::string cookie() const;

  // Custom, not in any spec: Node.
  //
  scoped_refptr<Document> AsDocument() OVERRIDE { return this; }

  void Accept(NodeVisitor* visitor) OVERRIDE;
  void Accept(ConstNodeVisitor* visitor) const OVERRIDE;

  scoped_refptr<Node> Duplicate() const OVERRIDE {
    return new Document(
        html_element_context_,
        Options(location_->url(), navigation_start_clock_,
                location_->navigation_callback(), user_agent_style_sheet_,
                viewport_size_, cookie_jar_, net_poster_factory_));
  }

  // Custom, not in any spec.
  //
  virtual bool IsXMLDocument() const { return false; }

  HTMLElementContext* html_element_context() const {
    return html_element_context_;
  }

  FontCache* font_cache() const { return font_cache_.get(); }

  const GURL& url_as_gurl() const { return location_->url(); }

  scoped_refptr<HTMLHtmlElement> html() const;

  // List of scripts that will execute in order as soon as possible.
  //   http://www.w3.org/TR/html5/scripting-1.html#list-of-scripts-that-will-execute-in-order-as-soon-as-possible
  std::deque<HTMLScriptElement*>* scripts_to_be_executed() {
    return &scripts_to_be_executed_;
  }

  cssom::SelectorTree* selector_tree() { return &selector_tree_; }

  // Returns a mapping from keyframes name to CSSKeyframesRule.  This can be
  // used to quickly lookup the @keyframes rule given a string identifier.
  const cssom::CSSKeyframesRule::NameMap& keyframes_map() const {
    return keyframes_map_;
  }

  // These functions are for setting weak references to certain elements in the
  // document.
  void SetBody(HTMLBodyElement* body);
  void SetHead(HTMLHeadElement* head);
  void SetHtml(HTMLHtmlElement* html);
  void SetActiveElement(Element* active_element);

  // Count all ongoing loadings, including document itself and its dependent
  // resources, and dispatch OnLoad() if necessary.
  void IncreaseLoadingCounter();
  void DecreaseLoadingCounterAndMaybeDispatchLoadEvent(bool load_succeeded);

  // Utilities related to DocumentObserver.
  void AddObserver(DocumentObserver* observer);
  void RemoveObserver(DocumentObserver* observer);
  void SignalOnLoadToObservers();

  // Must be called by all descendants of the document on their modification.
  // TODO(***REMOVED***): Provide more granularity, model after mutation observers
  //               (see http://www.w3.org/TR/dom/#mutation-observers).
  void RecordMutation();

  // From cssom::MutationObserver.
  void OnCSSMutation() OVERRIDE;

  // Called when the DOM is mutated in some way.
  void OnDOMMutation();

  // Called when a new font has been loaded.
  void OnFontLoadEvent();

  // Called when the inline style of an element is modified.
  void OnElementInlineStyleMutation();

  // Updates the cached matching rules on all the elements in the document.
  // Media rules will be reevaluated using given root_computed_style.
  // Only a subset of selectors is supported as specified here:
  //   http://***REMOVED***cobalt-css#heading=h.s82z8u3l3se
  // Those selectors that are supported are implemented after Selectors Level 4.
  //   http://www.w3.org/TR/selectors4/
  void UpdateMatchingRules();

  void UpdateMediaRules();

  // Updates the computed styles of all of this document's HTML elements.
  void UpdateComputedStyles();

  // Scans the user agent style sheet and all style sheets in the document's
  // style sheet list and updates the font faces available in the document.
  void UpdateFontFaces();

  // Scans the user agent style sheet and all style sheets in the document's
  // style sheet list and compiles/updates a set of all declared CSS
  // keyframes used to define CSS Animations.
  void UpdateKeyframes();

  // Manages the clock used by Web Animations.
  //     http://www.w3.org/TR/web-animations
  // This clock is also used for requestAnimationFrame() callbacks, according
  // to the specification above.
  void SampleTimelineTime();

  const scoped_refptr<base::Clock>& navigation_start_clock() const {
    return navigation_start_clock_;
  }

  CSPDelegate* csp_delegate() const { return csp_delegate_.get(); }

#if defined(ENABLE_PARTIAL_LAYOUT_CONTROL)
  bool partial_layout_is_enabled() { return partial_layout_is_enabled_; }
  void SetPartialLayout(const std::string& mode_string);
#endif  // defined(ENABLE_PARTIAL_LAYOUT_CONTROL)

  // Triggers a synchronous layout.
  void DoSynchronousLayout();
  void set_synchronous_layout_callback(
      const base::Closure& synchronous_layout_callback) {
    synchronous_layout_callback_ = synchronous_layout_callback;
  }

  void SetViewport(const math::Size& viewport_size);

  const scoped_refptr<cssom::CSSStyleDeclarationData>& root_computed_style()
      const {
    return root_computed_style_;
  }

  void NotifyUrlChanged(const GURL& url);
  const network_bridge::NetPosterFactory& net_poster_factory() const {
    return net_poster_factory_;
  }

  DEFINE_WRAPPABLE_TYPE(Document);

 protected:
  ~Document() OVERRIDE;

 private:
  void DispatchOnLoadEvent();

  // Reference to HTML element context.
  HTMLElementContext* const html_element_context_;
  // Associated DOM implementation object.
  scoped_refptr<DOMImplementation> implementation_;
  // Associated location object.
  scoped_refptr<Location> location_;

  // Creates NetPosters for use by CSPDelegate.
  network_bridge::NetPosterFactory net_poster_factory_;

  // Content Security Policy enforcement for this document.
  scoped_ptr<CSPDelegate> csp_delegate_;

  // List of CSS style sheets.
  scoped_refptr<cssom::StyleSheetList> style_sheets_;
  // List of scripts that will execute in order as soon as possible.
  std::deque<HTMLScriptElement*> scripts_to_be_executed_;
  // The font cache for this document.
  scoped_ptr<FontCache> font_cache_;
  // A mapping from keyframes declaration names to their parsed structure.
  cssom::CSSKeyframesRule::NameMap keyframes_map_;
  // The number of ongoing loadings.
  int loading_counter_;
  // Whether the load event should be dispatched when loading counter hits zero.
  bool should_dispatch_load_event_;
  // Indicates if rule matching/computed style is dirty and needs to be
  // recomputed before the next layout.
  bool is_selector_tree_dirty_;
  bool is_rule_matching_result_dirty_;
  bool is_computed_style_dirty_;
  bool are_font_faces_dirty_;
  bool are_keyframes_dirty_;

  // Weak references to the certain elements in the document.
  base::WeakPtr<HTMLBodyElement> body_;
  base::WeakPtr<HTMLHeadElement> head_;
  base::WeakPtr<HTMLHtmlElement> html_;
  base::WeakPtr<Element> active_element_;
  // List of document observers.
  ObserverList<DocumentObserver> observers_;
  // Selector Tree.
  cssom::SelectorTree selector_tree_;
  network_bridge::CookieJar* cookie_jar_;

  // The document's latest sample from the global clock, used for updating
  // animations.
  const scoped_refptr<base::Clock> navigation_start_clock_;
  scoped_refptr<DocumentTimeline> default_timeline_;

#if defined(ENABLE_PARTIAL_LAYOUT_CONTROL)
  bool partial_layout_is_enabled_;
#endif  // defined(ENABLE_PARTIAL_LAYOUT_CONTROL)

  base::Closure synchronous_layout_callback_;

  scoped_refptr<cssom::CSSStyleSheet> user_agent_style_sheet_;

  base::optional<math::Size> viewport_size_;
  scoped_refptr<cssom::CSSStyleDeclarationData> root_computed_style_;
};

}  // namespace dom
}  // namespace cobalt

#endif  // COBALT_DOM_DOCUMENT_H_

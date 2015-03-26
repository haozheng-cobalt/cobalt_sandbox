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

#include "cobalt/browser/web_module.h"

#include "base/logging.h"
#include "base/stringprintf.h"

namespace cobalt {
namespace browser {

std::string WebModule::GetUserAgent() {
#if defined(__LB_LINUX__)
  const char* kVendor = "NoVendor";
  const char* kPlatform = "Linux";
#elif defined(__LB_PS3__)
  const char* kVendor = "Sony";
  const char* kPlatform = "PS3";
#elif defined(COBALT_WIN)
  const char* kVendor = "Microsoft";
  const char* kPlatform = "Windows";
#else
#error Undefined platform
#endif

#if defined(COBALT_BUILD_TYPE_DEBUG)
  const char* kBuildType = "Debug";
#elif defined(COBALT_BUILD_TYPE_DEVEL)
  const char* kBuildType = "Devel";
#elif defined(COBALT_BUILD_TYPE_QA)
  const char* kBuildType = "QA";
#elif defined(COBALT_BUILD_TYPE_GOLD)
  const char* kBuildType = "Gold";
#else
#error Unknown build type
#endif

  const char* kChromiumVersion = "25.0.1364.70";
  const char* kCobaltVersion = "0.01";
  const char* kLanguageCode = "en";
  const char* kCountryCode = "US";

  std::string user_agent;
  std::string product =
      base::StringPrintf("Chrome/%s Cobalt/%s %s build", kChromiumVersion,
                         kCobaltVersion, kBuildType);
  user_agent.append(base::StringPrintf(
      "Mozilla/5.0 (%s) (KHTML, like Gecko) %s", kPlatform, product.c_str()));

  std::string vendor = kVendor;
  std::string device = kPlatform;
  std::string firmware_version = "";  // Okay to leave blank
  std::string model = kPlatform;
  std::string sku = "";  // Okay to leave blank
  std::string language_code = kLanguageCode;
  std::string country_code = kCountryCode;
  user_agent.append(base::StringPrintf(
      " %s %s/%s (%s, %s, %s, %s)", vendor.c_str(), device.c_str(),
      firmware_version.c_str(), model.c_str(), sku.c_str(),
      language_code.c_str(), country_code.c_str()));

  return user_agent;
}

WebModule::WebModule(const layout::LayoutManager::OnRenderTreeProducedCallback&
                         on_render_tree_produced,
                     const math::Size& window_dimensions,
                     render_tree::ResourceProvider* resource_provider,
                     const Options& options)
    : css_parser_(css_parser::Parser::Create()),
      javascript_engine_(script::JavaScriptEngine::CreateEngine()),
      global_object_proxy_(javascript_engine_->CreateGlobalObject()),
      script_runner_(
          script::ScriptRunner::CreateScriptRunner(global_object_proxy_)),
      fetcher_factory_(new loader::FetcherFactory()),
      window_(new dom::Window(window_dimensions.width(),
                              window_dimensions.height(), css_parser_.get(),
                              fetcher_factory_.get(), script_runner_.get(),
                              options.url, GetUserAgent())),
      layout_manager_(window_.get(), resource_provider, on_render_tree_produced,
                      css_parser_.get(), options.layout_trigger),
      media_module_(media::MediaModule::Create()) {
  global_object_proxy_->SetGlobalInterface(window_);
}

WebModule::~WebModule() {}

void WebModule::InjectEvent(const scoped_refptr<dom::Event>& event) {
  // Forward the input event on to the DOM document.
  thread_checker_.CalledOnValidThread();
  window_->document()->DispatchEvent(event);
}

}  // namespace browser
}  // namespace cobalt

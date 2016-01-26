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

#include "cobalt/network/network_module.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/stringprintf.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/thread.h"
#include "cobalt/network/network_system.h"
#include "cobalt/network/user_agent_string_factory.h"
#include "net/url_request/static_http_user_agent_settings.h"

namespace cobalt {
namespace network {

NetworkModule::NetworkModule() : storage_manager_(NULL) {
  Initialize(NULL /* event_dispatcher */);
}

NetworkModule::NetworkModule(const Options& options)
    : storage_manager_(NULL), options_(options) {
  Initialize(NULL /* event_dispatcher */);
}

NetworkModule::NetworkModule(storage::StorageManager* storage_manager,
                             base::EventDispatcher* event_dispatcher,
                             const Options& options)
    : storage_manager_(storage_manager), options_(options) {
  Initialize(event_dispatcher);
}

NetworkModule::~NetworkModule() {
  // Order of destruction is important here.
  // URLRequestContext and NetworkDelegate must be destroyed on the IO thread.
  // The ObjectWatchMultiplexer must be destroyed last.  (The sockets owned
  // by URLRequestContext will destroy their ObjectWatchers, which need the
  // multiplexer.)
  url_request_context_getter_ = NULL;
#if defined(DIAL_SERVER)
  dial_service_proxy_ = NULL;
  message_loop_proxy()->DeleteSoon(FROM_HERE, dial_service_.release());
#endif

  message_loop_proxy()->DeleteSoon(FROM_HERE, cookie_jar_.release());
  message_loop_proxy()->DeleteSoon(FROM_HERE, url_request_context_.release());
  message_loop_proxy()->DeleteSoon(FROM_HERE, network_delegate_.release());

  // This will run the above task, and then stop the thread.
  thread_.reset(NULL);
  object_watch_multiplexer_.reset(NULL);
  network_system_.reset(NULL);
}

const std::string& NetworkModule::GetUserAgent() const {
  return http_user_agent_settings_->GetUserAgent();
}

network_bridge::NetPosterFactory NetworkModule::GetNetPosterFactory() {
  return base::Bind(&NetworkModule::CreateNetPoster, base::Unretained(this));
}

void NetworkModule::SetProxy(const std::string& custom_proxy_rules) {
  message_loop_proxy()->PostTask(
      FROM_HERE, base::Bind(&URLRequestContext::SetProxy,
                            base::Unretained(url_request_context_.get()),
                            custom_proxy_rules));
}

void NetworkModule::Initialize(base::EventDispatcher* event_dispatcher) {
  thread_.reset(new base::Thread("NetworkModule"));
  object_watch_multiplexer_.reset(new base::ObjectWatchMultiplexer());
  network_system_ = NetworkSystem::Create(event_dispatcher);
  http_user_agent_settings_.reset(new net::StaticHttpUserAgentSettings(
      options_.preferred_language, "utf-8",
      UserAgentStringFactory::ForCurrentPlatform()->CreateUserAgentString()));

  // Launch the IO thread.
  base::Thread::Options thread_options;
  thread_options.message_loop_type = MessageLoop::TYPE_IO;
  thread_options.stack_size = 256 * 1024;
  thread_->StartWithOptions(thread_options);

  base::WaitableEvent creation_event(true, false);
  // Run Network module startup on IO thread,
  // so the network delegate and URL request context are
  // constructed on that thread.
  message_loop_proxy()->PostTask(
      FROM_HERE, base::Bind(&NetworkModule::OnCreate, base::Unretained(this),
                            &creation_event));
  // Wait for OnCreate() to run, so we can be sure our members
  // have been constructed.
  creation_event.Wait();
  DCHECK(url_request_context_);
  url_request_context_getter_ = new network::URLRequestContextGetter(
      url_request_context_.get(), thread_.get());
}

void NetworkModule::OnCreate(base::WaitableEvent* creation_event) {
  DCHECK(message_loop_proxy()->BelongsToCurrentThread());

  url_request_context_.reset(
      new URLRequestContext(storage_manager_, options_.custom_proxy,
                            options_.ignore_certificate_errors));
  network_delegate_.reset(
      new NetworkDelegate(options_.cookie_policy, options_.require_https));
  url_request_context_->set_http_user_agent_settings(
      http_user_agent_settings_.get());
  url_request_context_->set_network_delegate(network_delegate_.get());
  cookie_jar_.reset(new CookieJarImpl(url_request_context_->cookie_store()));
#if defined(DIAL_SERVER)
  dial_service_.reset(new net::DialService());
  dial_service_proxy_ = new net::DialServiceProxy(dial_service_->AsWeakPtr());
#endif

  creation_event->Signal();
}

scoped_ptr<network_bridge::NetPoster> NetworkModule::CreateNetPoster() {
  scoped_ptr<network_bridge::NetPoster> ret(new NetPoster(this));
  return ret.Pass();
}

}  // namespace network
}  // namespace cobalt

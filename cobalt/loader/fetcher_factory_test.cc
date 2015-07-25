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

#include <string>

#include "base/run_loop.h"
#include "cobalt/loader/fetcher_factory.h"
#include "cobalt/loader/file_fetcher.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cobalt {
namespace loader {
namespace {

class StubFetcherHandler : public Fetcher::Handler {
 public:
  explicit StubFetcherHandler(base::RunLoop* run_loop) : run_loop_(run_loop) {}

  // From Fetcher::Handler.
  void OnReceived(const char* data, size_t size) OVERRIDE {
    UNREFERENCED_PARAMETER(data);
    UNREFERENCED_PARAMETER(size);
  }
  void OnDone() OVERRIDE {
    if (run_loop_) {
      MessageLoop::current()->PostTask(FROM_HERE, run_loop_->QuitClosure());
    }
  }
  void OnError(const std::string& error) OVERRIDE {
    UNREFERENCED_PARAMETER(error);
    if (run_loop_) {
      MessageLoop::current()->PostTask(FROM_HERE, run_loop_->QuitClosure());
    }
  }

 private:
  base::RunLoop* run_loop_;
};

}  // namespace

class FetcherFactoryTest : public ::testing::Test {
 protected:
  FetcherFactoryTest()
      : message_loop_(MessageLoop::TYPE_DEFAULT),
        fetcher_factory_(NULL) {}
  ~FetcherFactoryTest() OVERRIDE {}

  MessageLoop message_loop_;
  FetcherFactory fetcher_factory_;
  scoped_ptr<Fetcher> fetcher_;
};

TEST_F(FetcherFactoryTest, InvalidURL) {
  StubFetcherHandler stub_fetcher_handler(NULL);
  fetcher_ = fetcher_factory_.CreateFetcher(GURL("invalid-url"),
                                            &stub_fetcher_handler);
  EXPECT_FALSE(fetcher_.get());
}

TEST_F(FetcherFactoryTest, EmptyURL) {
  StubFetcherHandler stub_fetcher_handler(NULL);
  fetcher_ =
      fetcher_factory_.CreateFetcher(GURL("file:///"), &stub_fetcher_handler);
  EXPECT_FALSE(fetcher_.get());
}

TEST_F(FetcherFactoryTest, MultipleCreations) {
  // Having a RunLoop ensures that any callback created by
  // FileFetcher will be able to run. We then quit the message loop in the
  // StubFetcherHandler when either OnDone() or OnError() has occurred.
  base::RunLoop run_loop;
  StubFetcherHandler stub_fetcher_handler(&run_loop);

  fetcher_ = fetcher_factory_.CreateFetcher(GURL("file:///nonempty-url-1"),
                                            &stub_fetcher_handler);
  EXPECT_TRUE(dynamic_cast<FileFetcher*>(fetcher_.get()));

  fetcher_ = fetcher_factory_.CreateFetcher(GURL("file:///nonempty-url-2"),
                                            &stub_fetcher_handler);
  EXPECT_TRUE(dynamic_cast<FileFetcher*>(fetcher_.get()));
  run_loop.Run();
}

}  // namespace loader
}  // namespace cobalt

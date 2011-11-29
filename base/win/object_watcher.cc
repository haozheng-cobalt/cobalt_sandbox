// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/win/object_watcher.h"

#include "base/bind.h"
#include "base/logging.h"

namespace base {
namespace win {

//-----------------------------------------------------------------------------

ObjectWatcher::ObjectWatcher()
    : weak_factory_(this),
      object_(NULL),
      wait_object_(NULL),
      origin_loop_(NULL),
      delegate_(NULL) {
}

ObjectWatcher::~ObjectWatcher() {
  StopWatching();
}

bool ObjectWatcher::StartWatching(HANDLE object, Delegate* delegate) {
  if (wait_object_) {
    NOTREACHED() << "Already watching an object";
    return false;
  }

  origin_loop_ = MessageLoop::current();
  delegate_ = delegate;

  // Since our job is to just notice when an object is signaled and report the
  // result back to this thread, we can just run on a Windows wait thread.
  DWORD wait_flags = WT_EXECUTEINWAITTHREAD | WT_EXECUTEONLYONCE;

  if (!RegisterWaitForSingleObject(&wait_object_, object, DoneWaiting,
                                   this, INFINITE, wait_flags)) {
    NOTREACHED() << "RegisterWaitForSingleObject failed: " << GetLastError();
    return false;
  }

  object_ = object;

  // We need to know if the current message loop is going away so we can
  // prevent the wait thread from trying to access a dead message loop.
  MessageLoop::current()->AddDestructionObserver(this);
  return true;
}

bool ObjectWatcher::StopWatching() {
  if (!wait_object_)
    return false;

  // Make sure ObjectWatcher is used in a single-threaded fashion.
  DCHECK(origin_loop_ == MessageLoop::current());

  // Blocking call to cancel the wait. Any callbacks already in progress will
  // finish before we return from this call.
  if (!UnregisterWaitEx(wait_object_, INVALID_HANDLE_VALUE)) {
    NOTREACHED() << "UnregisterWaitEx failed: " << GetLastError();
    return false;
  }

  weak_factory_.InvalidateWeakPtrs();
  object_ = NULL;
  wait_object_ = NULL;

  MessageLoop::current()->RemoveDestructionObserver(this);
  return true;
}

HANDLE ObjectWatcher::GetWatchedObject() {
  return object_;
}

// static
void CALLBACK ObjectWatcher::DoneWaiting(void* param, BOOLEAN timed_out) {
  DCHECK(!timed_out);

  // The destructor blocks on any callbacks that are in flight, so we know that
  // that is always a pointer to a valid ObjectWater.
  ObjectWatcher* that = static_cast<ObjectWatcher*>(param);
  that->origin_loop_->PostTask(
      FROM_HERE,
      base::Bind(&ObjectWatcher::Signal, that->weak_factory_.GetWeakPtr()));
}

void ObjectWatcher::Signal() {
  StopWatching();
  delegate_->OnObjectSignaled(object_);
}

void ObjectWatcher::WillDestroyCurrentMessageLoop() {
  // Need to shutdown the watch so that we don't try to access the MessageLoop
  // after this point.
  StopWatching();
}

}  // namespace win
}  // namespace base

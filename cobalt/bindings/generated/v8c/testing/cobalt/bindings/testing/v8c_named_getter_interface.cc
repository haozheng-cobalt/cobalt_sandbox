

// Copyright 2017 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// clang-format off

// This file has been auto-generated by bindings/code_generator_cobalt.py. DO NOT MODIFY!
// Auto-generated from template: bindings/v8c/templates/interface.cc.template

#include "cobalt/bindings/testing/v8c_named_getter_interface.h"

#include "base/debug/trace_event.h"
#include "cobalt/base/polymorphic_downcast.h"
#include "cobalt/script/global_environment.h"
#include "cobalt/script/script_value.h"
#include "cobalt/script/value_handle.h"

#include "v8c_gen_type_conversion.h"

#include "cobalt/script/callback_interface_traits.h"
#include "cobalt/script/v8c/callback_function_conversion.h"
#include "cobalt/script/v8c/conversion_helpers.h"
#include "cobalt/script/v8c/entry_scope.h"
#include "cobalt/script/v8c/native_promise.h"
#include "cobalt/script/v8c/type_traits.h"
#include "cobalt/script/v8c/v8c_callback_function.h"
#include "cobalt/script/v8c/v8c_callback_interface_holder.h"
#include "cobalt/script/v8c/v8c_exception_state.h"
#include "cobalt/script/v8c/v8c_global_environment.h"
#include "cobalt/script/v8c/v8c_value_handle.h"
#include "cobalt/script/v8c/wrapper_private.h"
#include "v8/include/v8.h"


namespace {
using cobalt::bindings::testing::NamedGetterInterface;
using cobalt::bindings::testing::V8cNamedGetterInterface;
using cobalt::script::CallbackInterfaceTraits;
using cobalt::script::GlobalEnvironment;
using cobalt::script::ScriptValue;
using cobalt::script::ValueHandle;
using cobalt::script::ValueHandle;
using cobalt::script::ValueHandleHolder;
using cobalt::script::Wrappable;

using cobalt::script::v8c::EntryScope;
using cobalt::script::v8c::EscapableEntryScope;
using cobalt::script::v8c::FromJSValue;
using cobalt::script::v8c::InterfaceData;
using cobalt::script::v8c::kConversionFlagClamped;
using cobalt::script::v8c::kConversionFlagNullable;
using cobalt::script::v8c::kConversionFlagObjectOnly;
using cobalt::script::v8c::kConversionFlagRestricted;
using cobalt::script::v8c::kConversionFlagTreatNullAsEmptyString;
using cobalt::script::v8c::kConversionFlagTreatUndefinedAsEmptyString;
using cobalt::script::v8c::kNoConversionFlags;
using cobalt::script::v8c::ToJSValue;
using cobalt::script::v8c::TypeTraits;
using cobalt::script::v8c::V8cExceptionState;
using cobalt::script::v8c::V8cGlobalEnvironment;
using cobalt::script::v8c::WrapperFactory;
using cobalt::script::v8c::WrapperPrivate;

v8::Local<v8::Object> DummyFunctor(v8::Isolate*, const scoped_refptr<Wrappable>&) {
  NOTIMPLEMENTED();
  return {};
}

}  // namespace

namespace cobalt {
namespace bindings {
namespace testing {


namespace {





void namedDeleterMethod(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Object> object = info.This();
  V8cGlobalEnvironment* global_environment = V8cGlobalEnvironment::GetFromIsolate(isolate);
  WrapperFactory* wrapper_factory = global_environment->wrapper_factory();
  if (!wrapper_factory->DoesObjectImplementInterface(
        object, base::GetTypeId<NamedGetterInterface>())) {
    V8cExceptionState exception(isolate);
    exception.SetSimpleException(script::kDoesNotImplementInterface);
    return;
  }
  V8cExceptionState exception_state{isolate};
  v8::Local<v8::Value> result_value;

  WrapperPrivate* wrapper_private =
      WrapperPrivate::GetFromWrapperObject(object);
  if (!wrapper_private) {
    NOTIMPLEMENTED();
    return;
  }
  NamedGetterInterface* impl =
      wrapper_private->wrappable<NamedGetterInterface>().get();
  const size_t kMinArguments = 1;
  if (info.Length() < kMinArguments) {
    exception_state.SetSimpleException(script::kInvalidNumberOfArguments);
    return;
  }
  // Non-optional arguments
  TypeTraits<std::string >::ConversionType name;
  DCHECK_LT(0, info.Length());
  v8::Local<v8::Value> non_optional_value0 = info[0];
  FromJSValue(isolate,
              non_optional_value0,
              kNoConversionFlags,
              &exception_state, &name);
  if (exception_state.is_exception_set()) {
    return;
  }

  impl->NamedDeleter(name);
  result_value = v8::Undefined(isolate);

}



void namedGetterMethod(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Object> object = info.This();
  V8cGlobalEnvironment* global_environment = V8cGlobalEnvironment::GetFromIsolate(isolate);
  WrapperFactory* wrapper_factory = global_environment->wrapper_factory();
  if (!wrapper_factory->DoesObjectImplementInterface(
        object, base::GetTypeId<NamedGetterInterface>())) {
    V8cExceptionState exception(isolate);
    exception.SetSimpleException(script::kDoesNotImplementInterface);
    return;
  }
  V8cExceptionState exception_state{isolate};
  v8::Local<v8::Value> result_value;

  WrapperPrivate* wrapper_private =
      WrapperPrivate::GetFromWrapperObject(object);
  if (!wrapper_private) {
    NOTIMPLEMENTED();
    return;
  }
  NamedGetterInterface* impl =
      wrapper_private->wrappable<NamedGetterInterface>().get();
  const size_t kMinArguments = 1;
  if (info.Length() < kMinArguments) {
    exception_state.SetSimpleException(script::kInvalidNumberOfArguments);
    return;
  }
  // Non-optional arguments
  TypeTraits<std::string >::ConversionType name;
  DCHECK_LT(0, info.Length());
  v8::Local<v8::Value> non_optional_value0 = info[0];
  FromJSValue(isolate,
              non_optional_value0,
              kNoConversionFlags,
              &exception_state, &name);
  if (exception_state.is_exception_set()) {
    return;
  }

  if (!exception_state.is_exception_set()) {
    ToJSValue(isolate,
              impl->NamedGetter(name),
              &result_value);
  }
  if (!exception_state.is_exception_set()) {
    info.GetReturnValue().Set(result_value);
  }

}



void namedSetterMethod(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Object> object = info.This();
  V8cGlobalEnvironment* global_environment = V8cGlobalEnvironment::GetFromIsolate(isolate);
  WrapperFactory* wrapper_factory = global_environment->wrapper_factory();
  if (!wrapper_factory->DoesObjectImplementInterface(
        object, base::GetTypeId<NamedGetterInterface>())) {
    V8cExceptionState exception(isolate);
    exception.SetSimpleException(script::kDoesNotImplementInterface);
    return;
  }
  V8cExceptionState exception_state{isolate};
  v8::Local<v8::Value> result_value;

  WrapperPrivate* wrapper_private =
      WrapperPrivate::GetFromWrapperObject(object);
  if (!wrapper_private) {
    NOTIMPLEMENTED();
    return;
  }
  NamedGetterInterface* impl =
      wrapper_private->wrappable<NamedGetterInterface>().get();
  const size_t kMinArguments = 2;
  if (info.Length() < kMinArguments) {
    exception_state.SetSimpleException(script::kInvalidNumberOfArguments);
    return;
  }
  // Non-optional arguments
  TypeTraits<std::string >::ConversionType name;
  TypeTraits<std::string >::ConversionType value;
  DCHECK_LT(0, info.Length());
  v8::Local<v8::Value> non_optional_value0 = info[0];
  FromJSValue(isolate,
              non_optional_value0,
              kNoConversionFlags,
              &exception_state, &name);
  if (exception_state.is_exception_set()) {
    return;
  }
  DCHECK_LT(1, info.Length());
  v8::Local<v8::Value> non_optional_value1 = info[1];
  FromJSValue(isolate,
              non_optional_value1,
              kNoConversionFlags,
              &exception_state, &value);
  if (exception_state.is_exception_set()) {
    return;
  }

  impl->NamedSetter(name, value);
  result_value = v8::Undefined(isolate);

}


void InitializeTemplateAndInterfaceObject(v8::Isolate* isolate, InterfaceData* interface_data) {
  v8::Local<v8::FunctionTemplate> function_template = v8::FunctionTemplate::New(isolate);
  function_template->SetClassName(
    v8::String::NewFromUtf8(isolate, "NamedGetterInterface",
        v8::NewStringType::kInternalized).ToLocalChecked());
  v8::Local<v8::ObjectTemplate> instance_template = function_template->InstanceTemplate();
  instance_template->SetInternalFieldCount(1);


  v8::Local<v8::ObjectTemplate> prototype_template = function_template->PrototypeTemplate();



  {
    v8::Local<v8::FunctionTemplate> method_template = v8::FunctionTemplate::New(isolate, namedDeleterMethod);
    method_template->RemovePrototype();
    prototype_template->Set(
        v8::String::NewFromUtf8(
            isolate,
            "namedDeleter",
            v8::NewStringType::kInternalized).ToLocalChecked(),
        method_template);
  }

  {
    v8::Local<v8::FunctionTemplate> method_template = v8::FunctionTemplate::New(isolate, namedGetterMethod);
    method_template->RemovePrototype();
    prototype_template->Set(
        v8::String::NewFromUtf8(
            isolate,
            "namedGetter",
            v8::NewStringType::kInternalized).ToLocalChecked(),
        method_template);
  }

  {
    v8::Local<v8::FunctionTemplate> method_template = v8::FunctionTemplate::New(isolate, namedSetterMethod);
    method_template->RemovePrototype();
    prototype_template->Set(
        v8::String::NewFromUtf8(
            isolate,
            "namedSetter",
            v8::NewStringType::kInternalized).ToLocalChecked(),
        method_template);
  }


  interface_data->function_template.Set(isolate, function_template);
}

inline InterfaceData* GetInterfaceData(V8cGlobalEnvironment* global_environment) {
  const int kInterfaceUniqueId = 31;
  // By convention, the |V8cGlobalEnvironment| that we are associated with
  // will hold our |InterfaceData| at index |kInterfaceUniqueId|, as we asked
  // for it to be there in the first place, and could not have conflicted with
  // any other interface.
  return global_environment->GetInterfaceData(kInterfaceUniqueId);
}

}  // namespace

v8::Local<v8::Object> V8cNamedGetterInterface::CreateWrapper(v8::Isolate* isolate, const scoped_refptr<Wrappable>& wrappable) {
  EscapableEntryScope entry_scope(isolate);
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  V8cGlobalEnvironment* global_environment = V8cGlobalEnvironment::GetFromIsolate(isolate);
  InterfaceData* interface_data = GetInterfaceData(global_environment);
  if (interface_data->function_template.IsEmpty()) {
    InitializeTemplateAndInterfaceObject(isolate, interface_data);
  }
  DCHECK(!interface_data->function_template.IsEmpty());

  v8::Local<v8::FunctionTemplate> function_template = interface_data->function_template.Get(isolate);
  DCHECK(function_template->InstanceTemplate()->InternalFieldCount() == 1);
  v8::Local<v8::Object> object = function_template->InstanceTemplate()->NewInstance(context).ToLocalChecked();
  DCHECK(object->InternalFieldCount() == 1);

  // This |WrapperPrivate|'s lifetime will be managed by V8.
  new WrapperPrivate(isolate, wrappable, object);
  return entry_scope.Escape(object);
}

v8::Local<v8::FunctionTemplate> V8cNamedGetterInterface::CreateTemplate(v8::Isolate* isolate) {
  V8cGlobalEnvironment* global_environment = V8cGlobalEnvironment::GetFromIsolate(isolate);
  InterfaceData* interface_data = GetInterfaceData(global_environment);
  if (interface_data->function_template.IsEmpty()) {
    InitializeTemplateAndInterfaceObject(isolate, interface_data);
  }

  return interface_data->function_template.Get(isolate);
}


}  // namespace testing
}  // namespace bindings
}  // namespace cobalt



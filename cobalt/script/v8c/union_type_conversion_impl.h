// This file was GENERATED by command:
//     pump.py union_type_conversion_impl.h.pump
// DO NOT EDIT BY HAND!!!

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

#ifndef COBALT_SCRIPT_V8C_UNION_TYPE_CONVERSION_IMPL_H_
#define COBALT_SCRIPT_V8C_UNION_TYPE_CONVERSION_IMPL_H_

#include "cobalt/base/type_id.h"
#include "cobalt/script/union_type.h"
#include "cobalt/script/v8c/type_traits.h"
#include "cobalt/script/v8c/v8c_exception_state.h"
#include "cobalt/script/v8c/v8c_global_environment.h"
#include "cobalt/script/v8c/v8c_user_object_holder.h"
#include "cobalt/script/v8c/v8c_value_handle.h"

// Conversion to/from Value for IDL union types.

namespace cobalt {
namespace script {
namespace v8c {

template <typename T1, typename T2>
void ToJSValue(v8::Isolate* isolate, const script::UnionType2<T1, T2>& in_union,
               v8::Local<v8::Value>* out_value) {
  if (in_union.template IsType<T1>()) {
    ToJSValue(isolate, in_union.template AsType<T1>(), out_value);
    return;
  }
  if (in_union.template IsType<T2>()) {
    ToJSValue(isolate, in_union.template AsType<T2>(), out_value);
    return;
  }
  NOTREACHED();
  *out_value = v8::Undefined(isolate);
}

template <typename T1, typename T2>
void FromJSValue(v8::Isolate* isolate, v8::Local<v8::Value> value,
                 int conversion_flags, ExceptionState* exception_state,
                 script::UnionType2<T1, T2>* out_union) {
  DCHECK_EQ(0, conversion_flags);
  // JS -> IDL type conversion procedure described here:
  // http://heycam.github.io/webidl/#es-union

  // TODO: Support Date, RegExp, DOMException, Error, ArrayBuffer, DataView,
  //       TypedArrayName, callback functions, dictionary.

  // 1. If the union type includes a nullable type and |V| is null or undefined,
  // then return the IDL value null.
  if (value->IsNullOrUndefined()) {
    // Since the nullability should have been detected by the conversion for
    // base::optional, we should throw, because if we get here it means the
    // union does not include a nullable type, but have been passed a null
    // value.
    exception_state->SetSimpleException(kNotUnionType);
    return;
  }

  // Typedef for readability.
  typedef ::cobalt::script::internal::UnionTypeTraits<T1> UnionTypeTraitsT1;
  typedef ::cobalt::script::internal::UnionTypeTraits<T2> UnionTypeTraitsT2;

  // 2. Let |types| be the flattened member types of the union type.
  // Forward declare all potential types
  T1 t1;
  T2 t2;

  // 4. If |V| is a platform object, then:
  //   1. If |types| includes an interface type that V implements, then return
  //      the IDL value that is a reference to the object |V|.
  //   2. If |types| includes object, then return the IDL value that is a
  //      reference to the object |V|.
  if (value->IsObject()) {
    // The specification doesn't dictate what should happen if V implements
    // more than one of the interfaces. For example, if V implements interface
    // B and interface B inherits from interface A, what happens if both A and
    // B are union members? Blink doesn't seem to do anything special for this
    // case. Just choose the first interface in the flattened members that
    // matches.

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> object = value->ToObject(context).ToLocalChecked();

    V8cGlobalEnvironment* global_environment =
        V8cGlobalEnvironment::GetFromIsolate(isolate);
    const WrapperFactory* wrapper_factory =
        global_environment->wrapper_factory();

    if (UnionTypeTraitsT1::is_interface_type &&
        wrapper_factory->DoesObjectImplementInterface(
            object, UnionTypeTraitsT1::GetTypeID())) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType2<T1, T2>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_interface_type &&
        wrapper_factory->DoesObjectImplementInterface(
            object, UnionTypeTraitsT2::GetTypeID())) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType2<T1, T2>(t2);
      return;
    }
  }

  // 11. If |Type(V)| is Object, then:
  //   1. If |types| includes a sequence type, then
  //     1. Let |method| be the result of GetMethod(V, @@iterator)
  //     2. ReturnIfAbrupt(method)
  //     3. If method is not undefined, return the result of creating a sequence
  //        of that type from |V| and |method|.
  if (value->IsObject()) {
    if (UnionTypeTraitsT1::is_sequence_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType2<T1, T2>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_sequence_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType2<T1, T2>(t2);
      return;
    }
  }

  // 12. If |Type(V)| is Boolean, then:
  // 1. If |types| includes a boolean, then return the result of converting |V|
  //      to boolean.
  if (value->IsBoolean()) {
    if (UnionTypeTraitsT1::is_boolean_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType2<T1, T2>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_boolean_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType2<T1, T2>(t2);
      return;
    }
  }

  // 13. If |Type(V)| is a Number, then:
  //   1. If |types| includes a numeric type, then return the result of
  //      converting |V| to that numeric type.
  if (value->IsNumber()) {
    if (UnionTypeTraitsT1::is_numeric_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType2<T1, T2>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_numeric_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType2<T1, T2>(t2);
      return;
    }
  }

  // 14. If |types| includes a string type, then return the result of converting
  //     |V| to that type.
  if (value->IsString()) {
    if (UnionTypeTraitsT1::is_string_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType2<T1, T2>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_string_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType2<T1, T2>(t2);
      return;
    }
  }

  // 15. If |types| includes a numeric type, then return the result of
  //     converting |V| to that numeric type.
  if (UnionTypeTraitsT1::is_numeric_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
    *out_union = script::UnionType2<T1, T2>(t1);
    return;
  }

  if (UnionTypeTraitsT2::is_numeric_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
    *out_union = script::UnionType2<T1, T2>(t2);
    return;
  }

  // 16. If |types| includes a boolean, then return the result of converting |V|
  //     to boolean.
  if (UnionTypeTraitsT1::is_boolean_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
    *out_union = script::UnionType2<T1, T2>(t1);
    return;
  }

  if (UnionTypeTraitsT2::is_boolean_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
    *out_union = script::UnionType2<T1, T2>(t2);
    return;
  }

  // 17. Throw a TypeError.
  exception_state->SetSimpleException(kNotUnionType);
}

template <typename T1, typename T2, typename T3>
void ToJSValue(v8::Isolate* isolate,
               const script::UnionType3<T1, T2, T3>& in_union,
               v8::Local<v8::Value>* out_value) {
  if (in_union.template IsType<T1>()) {
    ToJSValue(isolate, in_union.template AsType<T1>(), out_value);
    return;
  }
  if (in_union.template IsType<T2>()) {
    ToJSValue(isolate, in_union.template AsType<T2>(), out_value);
    return;
  }
  if (in_union.template IsType<T3>()) {
    ToJSValue(isolate, in_union.template AsType<T3>(), out_value);
    return;
  }
  NOTREACHED();
  *out_value = v8::Undefined(isolate);
}

template <typename T1, typename T2, typename T3>
void FromJSValue(v8::Isolate* isolate, v8::Local<v8::Value> value,
                 int conversion_flags, ExceptionState* exception_state,
                 script::UnionType3<T1, T2, T3>* out_union) {
  DCHECK_EQ(0, conversion_flags);
  // JS -> IDL type conversion procedure described here:
  // http://heycam.github.io/webidl/#es-union

  // TODO: Support Date, RegExp, DOMException, Error, ArrayBuffer, DataView,
  //       TypedArrayName, callback functions, dictionary.

  // 1. If the union type includes a nullable type and |V| is null or undefined,
  // then return the IDL value null.
  if (value->IsNullOrUndefined()) {
    // Since the nullability should have been detected by the conversion for
    // base::optional, we should throw, because if we get here it means the
    // union does not include a nullable type, but have been passed a null
    // value.
    exception_state->SetSimpleException(kNotUnionType);
    return;
  }

  // Typedef for readability.
  typedef ::cobalt::script::internal::UnionTypeTraits<T1> UnionTypeTraitsT1;
  typedef ::cobalt::script::internal::UnionTypeTraits<T2> UnionTypeTraitsT2;
  typedef ::cobalt::script::internal::UnionTypeTraits<T3> UnionTypeTraitsT3;

  // 2. Let |types| be the flattened member types of the union type.
  // Forward declare all potential types
  T1 t1;
  T2 t2;
  T3 t3;

  // 4. If |V| is a platform object, then:
  //   1. If |types| includes an interface type that V implements, then return
  //      the IDL value that is a reference to the object |V|.
  //   2. If |types| includes object, then return the IDL value that is a
  //      reference to the object |V|.
  if (value->IsObject()) {
    // The specification doesn't dictate what should happen if V implements
    // more than one of the interfaces. For example, if V implements interface
    // B and interface B inherits from interface A, what happens if both A and
    // B are union members? Blink doesn't seem to do anything special for this
    // case. Just choose the first interface in the flattened members that
    // matches.

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> object = value->ToObject(context).ToLocalChecked();

    V8cGlobalEnvironment* global_environment =
        V8cGlobalEnvironment::GetFromIsolate(isolate);
    const WrapperFactory* wrapper_factory =
        global_environment->wrapper_factory();

    if (UnionTypeTraitsT1::is_interface_type &&
        wrapper_factory->DoesObjectImplementInterface(
            object, UnionTypeTraitsT1::GetTypeID())) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType3<T1, T2, T3>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_interface_type &&
        wrapper_factory->DoesObjectImplementInterface(
            object, UnionTypeTraitsT2::GetTypeID())) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType3<T1, T2, T3>(t2);
      return;
    }

    if (UnionTypeTraitsT3::is_interface_type &&
        wrapper_factory->DoesObjectImplementInterface(
            object, UnionTypeTraitsT3::GetTypeID())) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
      *out_union = script::UnionType3<T1, T2, T3>(t3);
      return;
    }
  }

  // 11. If |Type(V)| is Object, then:
  //   1. If |types| includes a sequence type, then
  //     1. Let |method| be the result of GetMethod(V, @@iterator)
  //     2. ReturnIfAbrupt(method)
  //     3. If method is not undefined, return the result of creating a sequence
  //        of that type from |V| and |method|.
  if (value->IsObject()) {
    if (UnionTypeTraitsT1::is_sequence_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType3<T1, T2, T3>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_sequence_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType3<T1, T2, T3>(t2);
      return;
    }

    if (UnionTypeTraitsT3::is_sequence_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
      *out_union = script::UnionType3<T1, T2, T3>(t3);
      return;
    }
  }

  // 12. If |Type(V)| is Boolean, then:
  // 1. If |types| includes a boolean, then return the result of converting |V|
  //      to boolean.
  if (value->IsBoolean()) {
    if (UnionTypeTraitsT1::is_boolean_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType3<T1, T2, T3>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_boolean_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType3<T1, T2, T3>(t2);
      return;
    }

    if (UnionTypeTraitsT3::is_boolean_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
      *out_union = script::UnionType3<T1, T2, T3>(t3);
      return;
    }
  }

  // 13. If |Type(V)| is a Number, then:
  //   1. If |types| includes a numeric type, then return the result of
  //      converting |V| to that numeric type.
  if (value->IsNumber()) {
    if (UnionTypeTraitsT1::is_numeric_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType3<T1, T2, T3>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_numeric_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType3<T1, T2, T3>(t2);
      return;
    }

    if (UnionTypeTraitsT3::is_numeric_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
      *out_union = script::UnionType3<T1, T2, T3>(t3);
      return;
    }
  }

  // 14. If |types| includes a string type, then return the result of converting
  //     |V| to that type.
  if (value->IsString()) {
    if (UnionTypeTraitsT1::is_string_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType3<T1, T2, T3>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_string_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType3<T1, T2, T3>(t2);
      return;
    }

    if (UnionTypeTraitsT3::is_string_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
      *out_union = script::UnionType3<T1, T2, T3>(t3);
      return;
    }
  }

  // 15. If |types| includes a numeric type, then return the result of
  //     converting |V| to that numeric type.
  if (UnionTypeTraitsT1::is_numeric_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
    *out_union = script::UnionType3<T1, T2, T3>(t1);
    return;
  }

  if (UnionTypeTraitsT2::is_numeric_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
    *out_union = script::UnionType3<T1, T2, T3>(t2);
    return;
  }

  if (UnionTypeTraitsT3::is_numeric_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
    *out_union = script::UnionType3<T1, T2, T3>(t3);
    return;
  }

  // 16. If |types| includes a boolean, then return the result of converting |V|
  //     to boolean.
  if (UnionTypeTraitsT1::is_boolean_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
    *out_union = script::UnionType3<T1, T2, T3>(t1);
    return;
  }

  if (UnionTypeTraitsT2::is_boolean_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
    *out_union = script::UnionType3<T1, T2, T3>(t2);
    return;
  }

  if (UnionTypeTraitsT3::is_boolean_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
    *out_union = script::UnionType3<T1, T2, T3>(t3);
    return;
  }

  // 17. Throw a TypeError.
  exception_state->SetSimpleException(kNotUnionType);
}

template <typename T1, typename T2, typename T3, typename T4>
void ToJSValue(v8::Isolate* isolate,
               const script::UnionType4<T1, T2, T3, T4>& in_union,
               v8::Local<v8::Value>* out_value) {
  if (in_union.template IsType<T1>()) {
    ToJSValue(isolate, in_union.template AsType<T1>(), out_value);
    return;
  }
  if (in_union.template IsType<T2>()) {
    ToJSValue(isolate, in_union.template AsType<T2>(), out_value);
    return;
  }
  if (in_union.template IsType<T3>()) {
    ToJSValue(isolate, in_union.template AsType<T3>(), out_value);
    return;
  }
  if (in_union.template IsType<T4>()) {
    ToJSValue(isolate, in_union.template AsType<T4>(), out_value);
    return;
  }
  NOTREACHED();
  *out_value = v8::Undefined(isolate);
}

template <typename T1, typename T2, typename T3, typename T4>
void FromJSValue(v8::Isolate* isolate, v8::Local<v8::Value> value,
                 int conversion_flags, ExceptionState* exception_state,
                 script::UnionType4<T1, T2, T3, T4>* out_union) {
  DCHECK_EQ(0, conversion_flags);
  // JS -> IDL type conversion procedure described here:
  // http://heycam.github.io/webidl/#es-union

  // TODO: Support Date, RegExp, DOMException, Error, ArrayBuffer, DataView,
  //       TypedArrayName, callback functions, dictionary.

  // 1. If the union type includes a nullable type and |V| is null or undefined,
  // then return the IDL value null.
  if (value->IsNullOrUndefined()) {
    // Since the nullability should have been detected by the conversion for
    // base::optional, we should throw, because if we get here it means the
    // union does not include a nullable type, but have been passed a null
    // value.
    exception_state->SetSimpleException(kNotUnionType);
    return;
  }

  // Typedef for readability.
  typedef ::cobalt::script::internal::UnionTypeTraits<T1> UnionTypeTraitsT1;
  typedef ::cobalt::script::internal::UnionTypeTraits<T2> UnionTypeTraitsT2;
  typedef ::cobalt::script::internal::UnionTypeTraits<T3> UnionTypeTraitsT3;
  typedef ::cobalt::script::internal::UnionTypeTraits<T4> UnionTypeTraitsT4;

  // 2. Let |types| be the flattened member types of the union type.
  // Forward declare all potential types
  T1 t1;
  T2 t2;
  T3 t3;
  T4 t4;

  // 4. If |V| is a platform object, then:
  //   1. If |types| includes an interface type that V implements, then return
  //      the IDL value that is a reference to the object |V|.
  //   2. If |types| includes object, then return the IDL value that is a
  //      reference to the object |V|.
  if (value->IsObject()) {
    // The specification doesn't dictate what should happen if V implements
    // more than one of the interfaces. For example, if V implements interface
    // B and interface B inherits from interface A, what happens if both A and
    // B are union members? Blink doesn't seem to do anything special for this
    // case. Just choose the first interface in the flattened members that
    // matches.

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> object = value->ToObject(context).ToLocalChecked();

    V8cGlobalEnvironment* global_environment =
        V8cGlobalEnvironment::GetFromIsolate(isolate);
    const WrapperFactory* wrapper_factory =
        global_environment->wrapper_factory();

    if (UnionTypeTraitsT1::is_interface_type &&
        wrapper_factory->DoesObjectImplementInterface(
            object, UnionTypeTraitsT1::GetTypeID())) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_interface_type &&
        wrapper_factory->DoesObjectImplementInterface(
            object, UnionTypeTraitsT2::GetTypeID())) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t2);
      return;
    }

    if (UnionTypeTraitsT3::is_interface_type &&
        wrapper_factory->DoesObjectImplementInterface(
            object, UnionTypeTraitsT3::GetTypeID())) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t3);
      return;
    }

    if (UnionTypeTraitsT4::is_interface_type &&
        wrapper_factory->DoesObjectImplementInterface(
            object, UnionTypeTraitsT4::GetTypeID())) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t4);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t4);
      return;
    }
  }

  // 11. If |Type(V)| is Object, then:
  //   1. If |types| includes a sequence type, then
  //     1. Let |method| be the result of GetMethod(V, @@iterator)
  //     2. ReturnIfAbrupt(method)
  //     3. If method is not undefined, return the result of creating a sequence
  //        of that type from |V| and |method|.
  if (value->IsObject()) {
    if (UnionTypeTraitsT1::is_sequence_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_sequence_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t2);
      return;
    }

    if (UnionTypeTraitsT3::is_sequence_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t3);
      return;
    }

    if (UnionTypeTraitsT4::is_sequence_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t4);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t4);
      return;
    }
  }

  // 12. If |Type(V)| is Boolean, then:
  // 1. If |types| includes a boolean, then return the result of converting |V|
  //      to boolean.
  if (value->IsBoolean()) {
    if (UnionTypeTraitsT1::is_boolean_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_boolean_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t2);
      return;
    }

    if (UnionTypeTraitsT3::is_boolean_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t3);
      return;
    }

    if (UnionTypeTraitsT4::is_boolean_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t4);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t4);
      return;
    }
  }

  // 13. If |Type(V)| is a Number, then:
  //   1. If |types| includes a numeric type, then return the result of
  //      converting |V| to that numeric type.
  if (value->IsNumber()) {
    if (UnionTypeTraitsT1::is_numeric_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_numeric_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t2);
      return;
    }

    if (UnionTypeTraitsT3::is_numeric_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t3);
      return;
    }

    if (UnionTypeTraitsT4::is_numeric_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t4);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t4);
      return;
    }
  }

  // 14. If |types| includes a string type, then return the result of converting
  //     |V| to that type.
  if (value->IsString()) {
    if (UnionTypeTraitsT1::is_string_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t1);
      return;
    }

    if (UnionTypeTraitsT2::is_string_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t2);
      return;
    }

    if (UnionTypeTraitsT3::is_string_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t3);
      return;
    }

    if (UnionTypeTraitsT4::is_string_type) {
      FromJSValue(isolate, value, conversion_flags, exception_state, &t4);
      *out_union = script::UnionType4<T1, T2, T3, T4>(t4);
      return;
    }
  }

  // 15. If |types| includes a numeric type, then return the result of
  //     converting |V| to that numeric type.
  if (UnionTypeTraitsT1::is_numeric_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
    *out_union = script::UnionType4<T1, T2, T3, T4>(t1);
    return;
  }

  if (UnionTypeTraitsT2::is_numeric_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
    *out_union = script::UnionType4<T1, T2, T3, T4>(t2);
    return;
  }

  if (UnionTypeTraitsT3::is_numeric_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
    *out_union = script::UnionType4<T1, T2, T3, T4>(t3);
    return;
  }

  if (UnionTypeTraitsT4::is_numeric_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t4);
    *out_union = script::UnionType4<T1, T2, T3, T4>(t4);
    return;
  }

  // 16. If |types| includes a boolean, then return the result of converting |V|
  //     to boolean.
  if (UnionTypeTraitsT1::is_boolean_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t1);
    *out_union = script::UnionType4<T1, T2, T3, T4>(t1);
    return;
  }

  if (UnionTypeTraitsT2::is_boolean_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t2);
    *out_union = script::UnionType4<T1, T2, T3, T4>(t2);
    return;
  }

  if (UnionTypeTraitsT3::is_boolean_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t3);
    *out_union = script::UnionType4<T1, T2, T3, T4>(t3);
    return;
  }

  if (UnionTypeTraitsT4::is_boolean_type) {
    FromJSValue(isolate, value, conversion_flags, exception_state, &t4);
    *out_union = script::UnionType4<T1, T2, T3, T4>(t4);
    return;
  }

  // 17. Throw a TypeError.
  exception_state->SetSimpleException(kNotUnionType);
}

}  // namespace v8c
}  // namespace script
}  // namespace cobalt

#endif  // COBALT_SCRIPT_V8C_UNION_TYPE_CONVERSION_IMPL_H_

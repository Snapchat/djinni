/**
  * Copyright 2021 Snap, Inc.
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *    http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

#pragma once

#include "djinni_support.hpp"
#include "../cpp/DataView.hpp"

namespace djinni {

struct NativeDataView {
    using CppType = DataView;
    using JniType = jobject;

    static CppType toCpp(JNIEnv* jniEnv, JniType o) {
        auto size = jniEnv->GetDirectBufferCapacity(o);
        if (size == -1) {
            // GetDirectBufferCapacity() returns -1 when the ByteBuffer is not direct
            throw std::invalid_argument("ByteBuffer is not allocated with allocateDirect()");
        }
        return DataView(reinterpret_cast<uint8_t*>(jniEnv->GetDirectBufferAddress(o)), static_cast<size_t>(size));
    }

    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        jobject bufObj = jniEnv->NewDirectByteBuffer(c.buf(), c.len());
        return ::djinni::LocalRef<JniType>(bufObj);
    }

    using Boxed = NativeDataView;
};

} // namespace djinni

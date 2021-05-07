#pragma once

#include "djinni_support.hpp"
#include "DataView.hpp"

namespace snapchat {
namespace djinni {

struct NativeDataView {
    using CppType = DataView;
    using JniType = jobject;

    static CppType toCpp(JNIEnv* jniEnv, JniType o) {
        return DataView((uint8_t*)jniEnv->GetDirectBufferAddress(o), (size_t)jniEnv->GetDirectBufferCapacity(o));
    }

    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        jobject bufObj = jniEnv->NewDirectByteBuffer(c.buf(), c.len());
        return ::djinni::LocalRef<JniType>(bufObj);
    }

    using Boxed = NativeDataView;
};

}} // namespace snapchat::djinni

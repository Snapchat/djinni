#pragma once

#include "djinni_support.hpp"
#include "DataView.hpp"

namespace snapchat {
namespace djinni {

struct NativeDataView {
    using CppType = DataView;
    using JniType = jobject;

    static CppType toCpp(JNIEnv* jniEnv, JniType o) {
        auto size = jniEnv->GetDirectBufferCapacity(o);
        assert(size != -1); // GetDirectBufferCapacity() returns -1 when the ByteBuffer is not direct
        return DataView(reinterpret_cast<uint8_t*>(jniEnv->GetDirectBufferAddress(o)), static_cast<size_t>(size));
    }

    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        jobject bufObj = jniEnv->NewDirectByteBuffer(c.buf(), c.len());
        return ::djinni::LocalRef<JniType>(bufObj);
    }

    using Boxed = NativeDataView;
};

}} // namespace snapchat::djinni

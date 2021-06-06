// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from nscopying.djinni

#pragma once

#include "djinni_support.hpp"
#include "support_copying.hpp"

namespace djinni_generated {

class NativeSupportCopying final {
public:
    using CppType = ::testsuite::SupportCopying;
    using JniType = jobject;

    using Boxed = NativeSupportCopying;

    ~NativeSupportCopying();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    NativeSupportCopying();
    friend ::djinni::JniClass<NativeSupportCopying>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/dropbox/djinni/test/SupportCopying") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(I)V") };
    const jfieldID field_mX { ::djinni::jniGetFieldID(clazz.get(), "mX", "I") };
};

}  // namespace djinni_generated

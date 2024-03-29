// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from varnames.djinni

#pragma once

#include "_varname_record_.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class NativeVarnameRecord final {
public:
    using CppType = ::testsuite::VarnameRecord;
    using JniType = jobject;

    using Boxed = NativeVarnameRecord;

    ~NativeVarnameRecord();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    NativeVarnameRecord();
    friend ::djinni::JniClass<NativeVarnameRecord>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/dropbox/djinni/test/VarnameRecord") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(B)V") };
    const jfieldID field_mField { ::djinni::jniGetFieldID(clazz.get(), "mField", "B") };
};

} // namespace djinni_generated

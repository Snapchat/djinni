// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from static_method_language.djinni

#pragma once

#include "djinni_support.hpp"
#include "test_static_method_language.hpp"

namespace djinni_generated {

class NativeTestStaticMethodLanguage final : ::djinni::JniInterface<::testsuite::TestStaticMethodLanguage, NativeTestStaticMethodLanguage> {
public:
    using CppType = std::shared_ptr<::testsuite::TestStaticMethodLanguage>;
    using CppOptType = std::shared_ptr<::testsuite::TestStaticMethodLanguage>;
    using JniType = jobject;

    using Boxed = NativeTestStaticMethodLanguage;

    ~NativeTestStaticMethodLanguage();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) {
        DJINNI_ASSERT_MSG(j, jniEnv, "NativeTestStaticMethodLanguage::toCpp requires a non-null Java object");
        return ::djinni::JniClass<NativeTestStaticMethodLanguage>::get()._fromJava(jniEnv, j);
    };
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeTestStaticMethodLanguage>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        DJINNI_ASSERT_MSG(c, jniEnv, "NativeTestStaticMethodLanguage::fromCpp requires a non-null C++ object");
        return fromCppOpt(jniEnv, c); }
    }

private:
    NativeTestStaticMethodLanguage();
    friend ::djinni::JniClass<NativeTestStaticMethodLanguage>;
    friend ::djinni::JniInterface<::testsuite::TestStaticMethodLanguage, NativeTestStaticMethodLanguage>;

};

}  // namespace djinni_generated

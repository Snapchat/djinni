// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from duration.djinni

#pragma once

#include "djinni_support.hpp"
#include "test_duration.hpp"

namespace djinni_generated {

class NativeTestDuration final : ::djinni::JniInterface<::testsuite::TestDuration, NativeTestDuration> {
public:
    using CppType = std::shared_ptr<::testsuite::TestDuration>;
    using CppOptType = std::shared_ptr<::testsuite::TestDuration>;
    using JniType = jobject;

    using Boxed = NativeTestDuration;

    ~NativeTestDuration();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) {
        DJINNI_ASSERT_MSG(j, jniEnv, "NativeTestDuration::toCpp requires a non-null Java object");
        return ::djinni::JniClass<$jniSelf>::get()._fromJava(jniEnv, j);
    };
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeTestDuration>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        DJINNI_ASSERT_MSG(c, jniEnv, "NativeTestDuration::fromCpp requires a non-null C++ object");
        return fromCppOpt(jniEnv, c); }
    }

private:
    NativeTestDuration();
    friend ::djinni::JniClass<NativeTestDuration>;
    friend ::djinni::JniInterface<::testsuite::TestDuration, NativeTestDuration>;

};

}  // namespace djinni_generated

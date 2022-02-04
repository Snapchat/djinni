// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from multiple_inheritance.djinni

#pragma once

#include "djinni_support.hpp"
#include "return_one.hpp"

namespace djinni_generated {

class NativeReturnOne final : ::djinni::JniInterface<::testsuite::ReturnOne, NativeReturnOne> {
public:
    using CppType = std::shared_ptr<::testsuite::ReturnOne>;
    using CppOptType = std::shared_ptr<::testsuite::ReturnOne>;
    using JniType = jobject;

    using Boxed = NativeReturnOne;

    ~NativeReturnOne();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) {
        DJINNI_ASSERT_MSG(j, jniEnv, "NativeReturnOne::toCpp requires a non-null Java object");
        return ::djinni::JniClass<NativeReturnOne>::get()._fromJava(jniEnv, j);
    };
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeReturnOne>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        DJINNI_ASSERT_MSG(c, jniEnv, "NativeReturnOne::fromCpp requires a non-null C++ object");
        return fromCppOpt(jniEnv, c); }
    }

private:
    NativeReturnOne();
    friend ::djinni::JniClass<NativeReturnOne>;
    friend ::djinni::JniInterface<::testsuite::ReturnOne, NativeReturnOne>;

};

}  // namespace djinni_generated

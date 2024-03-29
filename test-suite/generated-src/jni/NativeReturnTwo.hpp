// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from multiple_inheritance.djinni

#pragma once

#include "djinni_support.hpp"
#include "return_two.hpp"

namespace djinni_generated {

class NativeReturnTwo final : ::djinni::JniInterface<::testsuite::ReturnTwo, NativeReturnTwo> {
public:
    using CppType = std::shared_ptr<::testsuite::ReturnTwo>;
    using CppOptType = std::shared_ptr<::testsuite::ReturnTwo>;
    using JniType = jobject;

    using Boxed = NativeReturnTwo;

    ~NativeReturnTwo();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<NativeReturnTwo>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeReturnTwo>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    NativeReturnTwo();
    friend ::djinni::JniClass<NativeReturnTwo>;
    friend ::djinni::JniInterface<::testsuite::ReturnTwo, NativeReturnTwo>;

};

} // namespace djinni_generated

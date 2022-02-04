// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from multiple_inheritance.djinni

#pragma once

#include "djinni_support.hpp"
#include "listener_caller.hpp"

namespace djinni_generated {

class NativeListenerCaller final : ::djinni::JniInterface<::testsuite::ListenerCaller, NativeListenerCaller> {
public:
    using CppType = std::shared_ptr<::testsuite::ListenerCaller>;
    using CppOptType = std::shared_ptr<::testsuite::ListenerCaller>;
    using JniType = jobject;

    using Boxed = NativeListenerCaller;

    ~NativeListenerCaller();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) {
        DJINNI_ASSERT_MSG(j, jniEnv, "NativeListenerCaller::toCpp requires a non-null Java object");
        return ::djinni::JniClass<NativeListenerCaller>::get()._fromJava(jniEnv, j);
    };
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeListenerCaller>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        DJINNI_ASSERT_MSG(c, jniEnv, "NativeListenerCaller::fromCpp requires a non-null C++ object");
        return fromCppOpt(jniEnv, c);
    }

private:
    NativeListenerCaller();
    friend ::djinni::JniClass<NativeListenerCaller>;
    friend ::djinni::JniInterface<::testsuite::ListenerCaller, NativeListenerCaller>;

};

}  // namespace djinni_generated

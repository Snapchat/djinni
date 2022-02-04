// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from single_language_interfaces.djinni

#pragma once

#include "djinni_support.hpp"
#include "java_only_listener.hpp"

namespace djinni_generated {

class NativeJavaOnlyListener final : ::djinni::JniInterface<::testsuite::JavaOnlyListener, NativeJavaOnlyListener> {
public:
    using CppType = std::shared_ptr<::testsuite::JavaOnlyListener>;
    using CppOptType = std::shared_ptr<::testsuite::JavaOnlyListener>;
    using JniType = jobject;

    using Boxed = NativeJavaOnlyListener;

    ~NativeJavaOnlyListener();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) {
        DJINNI_ASSERT_MSG(j, jniEnv, "NativeJavaOnlyListener::toCpp requires a non-null Java object");
        return ::djinni::JniClass<NativeJavaOnlyListener>::get()._fromJava(jniEnv, j);
    };
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeJavaOnlyListener>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        DJINNI_ASSERT_MSG(c, jniEnv, "NativeJavaOnlyListener::fromCpp requires a non-null C++ object");
        return fromCppOpt(jniEnv, c);
    }

private:
    NativeJavaOnlyListener();
    friend ::djinni::JniClass<NativeJavaOnlyListener>;
    friend ::djinni::JniInterface<::testsuite::JavaOnlyListener, NativeJavaOnlyListener>;

    class JavaProxy final : ::djinni::JavaProxyHandle<JavaProxy>, public ::testsuite::JavaOnlyListener
    {
    public:
        JavaProxy(JniType j);
        ~JavaProxy();


    private:
        friend ::djinni::JniInterface<::testsuite::JavaOnlyListener, ::djinni_generated::NativeJavaOnlyListener>;
    };

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/dropbox/djinni/test/JavaOnlyListener") };
};

}  // namespace djinni_generated

// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from user_token.djinni

#pragma once

#include "djinni_support.hpp"
#include "user_token.hpp"

namespace djinni_generated {

class NativeUserToken final : ::djinni::JniInterface<::testsuite::UserToken, NativeUserToken> {
public:
    using CppType = std::shared_ptr<::testsuite::UserToken>;
    using CppOptType = std::shared_ptr<::testsuite::UserToken>;
    using JniType = jobject;

    using Boxed = NativeUserToken;

    ~NativeUserToken();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<NativeUserToken>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeUserToken>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    NativeUserToken();
    friend ::djinni::JniClass<NativeUserToken>;
    friend ::djinni::JniInterface<::testsuite::UserToken, NativeUserToken>;

    class JavaProxy final : ::djinni::JavaProxyHandle<JavaProxy>, public ::testsuite::UserToken
    {
    public:
        JavaProxy(JniType j);
        ~JavaProxy();

        std::string whoami() override;

    private:
        friend ::djinni::JniInterface<::testsuite::UserToken, ::djinni_generated::NativeUserToken>;
    };

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/dropbox/djinni/test/UserToken") };
    const jmethodID method_whoami { ::djinni::jniGetMethodID(clazz.get(), "whoami", "()Ljava/lang/String;") };
};

}  // namespace djinni_generated

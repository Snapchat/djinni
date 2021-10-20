#pragma once

#include "async_interface.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class NativeAsyncInterface final : ::djinni::JniInterface<::testsuite::AsyncInterface, NativeAsyncInterface> {
public:
    using CppType = std::shared_ptr<::testsuite::AsyncInterface>;
    using CppOptType = std::shared_ptr<::testsuite::AsyncInterface>;
    using JniType = jobject;

    using Boxed = NativeAsyncInterface;

    ~NativeAsyncInterface();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<NativeAsyncInterface>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeAsyncInterface>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    NativeAsyncInterface();

    friend ::djinni::JniClass<NativeAsyncInterface>;
    friend ::djinni::JniInterface<::testsuite::AsyncInterface, NativeAsyncInterface>;

    class JavaProxy final : ::djinni::JavaProxyHandle<JavaProxy>, public ::testsuite::AsyncInterface
    {
    public:
        JavaProxy(JniType j);
        ~JavaProxy();

        ::djinni::Future<int32_t> get_async_result() override;

    private:
        friend ::djinni::JniInterface<::testsuite::AsyncInterface, ::djinni_generated::NativeAsyncInterface>;
    };

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/dropbox/djinni/test/AsyncInterface") };
    const jmethodID method_getAsyncResult { ::djinni::jniGetMethodID(clazz.get(), "getAsyncResult", "()Lcom/snapchat/djinni/Future;") };
};

}

// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from single_language_interfaces.djinni

#pragma once

#include "djinni_support.hpp"
#include "uses_single_language_listeners.hpp"

namespace djinni_generated {

class NativeUsesSingleLanguageListeners final : ::djinni::JniInterface<::testsuite::UsesSingleLanguageListeners, NativeUsesSingleLanguageListeners> {
public:
    using CppType = std::shared_ptr<::testsuite::UsesSingleLanguageListeners>;
    using CppOptType = std::shared_ptr<::testsuite::UsesSingleLanguageListeners>;
    using JniType = jobject;

    using Boxed = NativeUsesSingleLanguageListeners;

    ~NativeUsesSingleLanguageListeners();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<NativeUsesSingleLanguageListeners>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeUsesSingleLanguageListeners>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    NativeUsesSingleLanguageListeners();
    friend ::djinni::JniClass<NativeUsesSingleLanguageListeners>;
    friend ::djinni::JniInterface<::testsuite::UsesSingleLanguageListeners, NativeUsesSingleLanguageListeners>;

    class JavaProxy final : ::djinni::JavaProxyHandle<JavaProxy>, public ::testsuite::UsesSingleLanguageListeners
    {
    public:
        JavaProxy(JniType j);
        ~JavaProxy();

        void callForObjC(const /*not-null*/ std::shared_ptr<::testsuite::ObjcOnlyListener> & l) override;
        /*not-null*/ std::shared_ptr<::testsuite::ObjcOnlyListener> returnForObjC() override;
        void callForJava(const /*not-null*/ std::shared_ptr<::testsuite::JavaOnlyListener> & l) override;
        /*not-null*/ std::shared_ptr<::testsuite::JavaOnlyListener> returnForJava() override;

    private:
        friend ::djinni::JniInterface<::testsuite::UsesSingleLanguageListeners, ::djinni_generated::NativeUsesSingleLanguageListeners>;
    };

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/dropbox/djinni/test/UsesSingleLanguageListeners") };
    const jmethodID method_callForObjC { ::djinni::jniGetMethodID(clazz.get(), "callForObjC", "(Lcom/dropbox/djinni/test/ObjcOnlyListener;)V") };
    const jmethodID method_returnForObjC { ::djinni::jniGetMethodID(clazz.get(), "returnForObjC", "()Lcom/dropbox/djinni/test/ObjcOnlyListener;") };
    const jmethodID method_callForJava { ::djinni::jniGetMethodID(clazz.get(), "callForJava", "(Lcom/dropbox/djinni/test/JavaOnlyListener;)V") };
    const jmethodID method_returnForJava { ::djinni::jniGetMethodID(clazz.get(), "returnForJava", "()Lcom/dropbox/djinni/test/JavaOnlyListener;") };
};

} // namespace djinni_generated

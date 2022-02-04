// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from yaml-test.djinni

#pragma once

#include "djinni_support.hpp"
#include "extern_interface_2.hpp"

namespace djinni_generated {

class NativeExternInterface2 final : ::djinni::JniInterface<::ExternInterface2, NativeExternInterface2> {
public:
    using CppType = std::shared_ptr<::ExternInterface2>;
    using CppOptType = std::shared_ptr<::ExternInterface2>;
    using JniType = jobject;

    using Boxed = NativeExternInterface2;

    ~NativeExternInterface2();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) {
        DJINNI_ASSERT_MSG(j, jniEnv, "NativeExternInterface2::toCpp requires a non-null Java object");
        return ::djinni::JniClass<$jniSelf>::get()._fromJava(jniEnv, j);
    };
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeExternInterface2>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        DJINNI_ASSERT_MSG(c, jniEnv, "NativeExternInterface2::fromCpp requires a non-null C++ object");
        return fromCppOpt(jniEnv, c); }
    }

private:
    NativeExternInterface2();
    friend ::djinni::JniClass<NativeExternInterface2>;
    friend ::djinni::JniInterface<::ExternInterface2, NativeExternInterface2>;

    class JavaProxy final : ::djinni::JavaProxyHandle<JavaProxy>, public ::ExternInterface2
    {
    public:
        JavaProxy(JniType j);
        ~JavaProxy();

        ::ExternRecordWithDerivings foo(const /*not-null*/ std::shared_ptr<::testsuite::TestHelpers> & i) override;

    private:
        friend ::djinni::JniInterface<::ExternInterface2, ::djinni_generated::NativeExternInterface2>;
    };

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/dropbox/djinni/test/ExternInterface2") };
    const jmethodID method_foo { ::djinni::jniGetMethodID(clazz.get(), "foo", "(Lcom/dropbox/djinni/test/TestHelpers;)Lcom/dropbox/djinni/test/ExternRecordWithDerivings;") };
};

}  // namespace djinni_generated

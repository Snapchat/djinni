// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from single_language_interfaces.djinni

#pragma once

#include "djinni_support.hpp"
#include "objc_only_listener.hpp"

namespace djinni_generated {

class NativeObjcOnlyListener final : ::djinni::JniInterface<::testsuite::ObjcOnlyListener, NativeObjcOnlyListener> {
public:
    using CppType = std::shared_ptr<::testsuite::ObjcOnlyListener>;
    using CppOptType = std::shared_ptr<::testsuite::ObjcOnlyListener>;
    using JniType = jobject;

    using Boxed = NativeObjcOnlyListener;

    ~NativeObjcOnlyListener();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) {
        DJINNI_ASSERT_MSG(j, jniEnv, "NativeObjcOnlyListener::toCpp requires a non-null Java object");
        return ::djinni::JniClass<$jniSelf>::get()._fromJava(jniEnv, j);
    };
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeObjcOnlyListener>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        DJINNI_ASSERT_MSG(c, jniEnv, "NativeObjcOnlyListener::fromCpp requires a non-null C++ object");
        return fromCppOpt(jniEnv, c); }
    }

private:
    NativeObjcOnlyListener();
    friend ::djinni::JniClass<NativeObjcOnlyListener>;
    friend ::djinni::JniInterface<::testsuite::ObjcOnlyListener, NativeObjcOnlyListener>;

};

}  // namespace djinni_generated

// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from test.djinni

#pragma once

#include "Conflict.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class NativeConflict final : ::djinni::JniInterface<::testsuite::Conflict, NativeConflict> {
public:
    using CppType = std::shared_ptr<::testsuite::Conflict>;
    using CppOptType = std::shared_ptr<::testsuite::Conflict>;
    using JniType = jobject;

    using Boxed = NativeConflict;

    ~NativeConflict();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) {
        DJINNI_ASSERT_MSG(j, jniEnv, "NativeConflict::toCpp requires a non-null Java object");
        return ::djinni::JniClass<NativeConflict>::get()._fromJava(jniEnv, j);
    };
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeConflict>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        DJINNI_ASSERT_MSG(c, jniEnv, "NativeConflict::fromCpp requires a non-null C++ object");
        return fromCppOpt(jniEnv, c); }
    }

private:
    NativeConflict();
    friend ::djinni::JniClass<NativeConflict>;
    friend ::djinni::JniInterface<::testsuite::Conflict, NativeConflict>;

};

}  // namespace djinni_generated

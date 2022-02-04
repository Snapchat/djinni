// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from wchar_test.djinni

#pragma once

#include "djinni_support.hpp"
#include "wchar_test_helpers.hpp"

namespace djinni_generated {

class NativeWcharTestHelpers final : ::djinni::JniInterface<::testsuite::WcharTestHelpers, NativeWcharTestHelpers> {
public:
    using CppType = std::shared_ptr<::testsuite::WcharTestHelpers>;
    using CppOptType = std::shared_ptr<::testsuite::WcharTestHelpers>;
    using JniType = jobject;

    using Boxed = NativeWcharTestHelpers;

    ~NativeWcharTestHelpers();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) {
        DJINNI_ASSERT_MSG(j, jniEnv, "NativeWcharTestHelpers::toCpp requires a non-null Java object");
        return ::djinni::JniClass<NativeWcharTestHelpers>::get()._fromJava(jniEnv, j);
    };
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeWcharTestHelpers>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        DJINNI_ASSERT_MSG(c, jniEnv, "NativeWcharTestHelpers::fromCpp requires a non-null C++ object");
        return fromCppOpt(jniEnv, c); }
    }

private:
    NativeWcharTestHelpers();
    friend ::djinni::JniClass<NativeWcharTestHelpers>;
    friend ::djinni::JniInterface<::testsuite::WcharTestHelpers, NativeWcharTestHelpers>;

};

}  // namespace djinni_generated

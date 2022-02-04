// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from constants.djinni

#pragma once

#include "constants_interface.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class NativeConstantsInterface final : ::djinni::JniInterface<::testsuite::ConstantsInterface, NativeConstantsInterface> {
public:
    using CppType = std::shared_ptr<::testsuite::ConstantsInterface>;
    using CppOptType = std::shared_ptr<::testsuite::ConstantsInterface>;
    using JniType = jobject;

    using Boxed = NativeConstantsInterface;

    ~NativeConstantsInterface();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) {
        DJINNI_ASSERT_MSG(j, jniEnv, "NativeConstantsInterface::toCpp requires a non-null Java object");
        return ::djinni::JniClass<NativeConstantsInterface>::get()._fromJava(jniEnv, j);
    };
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeConstantsInterface>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        DJINNI_ASSERT_MSG(c, jniEnv, "NativeConstantsInterface::fromCpp requires a non-null C++ object");
        return fromCppOpt(jniEnv, c);
    }

private:
    NativeConstantsInterface();
    friend ::djinni::JniClass<NativeConstantsInterface>;
    friend ::djinni::JniInterface<::testsuite::ConstantsInterface, NativeConstantsInterface>;

};

}  // namespace djinni_generated

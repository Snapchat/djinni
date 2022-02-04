// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from proto.djinni

#pragma once

#include "djinni_support.hpp"
#include "proto_tests.hpp"

namespace djinni_generated {

class NativeProtoTests final : ::djinni::JniInterface<::testsuite::ProtoTests, NativeProtoTests> {
public:
    using CppType = std::shared_ptr<::testsuite::ProtoTests>;
    using CppOptType = std::shared_ptr<::testsuite::ProtoTests>;
    using JniType = jobject;

    using Boxed = NativeProtoTests;

    ~NativeProtoTests();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) {
        DJINNI_ASSERT_MSG(j, jniEnv, "NativeProtoTests::toCpp requires a non-null Java object");
        return ::djinni::JniClass<NativeProtoTests>::get()._fromJava(jniEnv, j);
    };
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeProtoTests>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        DJINNI_ASSERT_MSG(c, jniEnv, "NativeProtoTests::fromCpp requires a non-null C++ object");
        return fromCppOpt(jniEnv, c);
    }

private:
    NativeProtoTests();
    friend ::djinni::JniClass<NativeProtoTests>;
    friend ::djinni::JniInterface<::testsuite::ProtoTests, NativeProtoTests>;

};

}  // namespace djinni_generated

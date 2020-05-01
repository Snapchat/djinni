// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from function_prologue.djinni

#pragma once

#include "FunctionPrologueHelper.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class NativeFunctionPrologueHelper final : ::djinni::JniInterface<::testsuite::FunctionPrologueHelper, NativeFunctionPrologueHelper> {
public:
    using CppType = std::shared_ptr<::testsuite::FunctionPrologueHelper>;
    using CppOptType = std::shared_ptr<::testsuite::FunctionPrologueHelper>;
    using JniType = jobject;

    using Boxed = NativeFunctionPrologueHelper;

    ~NativeFunctionPrologueHelper();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<NativeFunctionPrologueHelper>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeFunctionPrologueHelper>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    NativeFunctionPrologueHelper();
    friend ::djinni::JniClass<NativeFunctionPrologueHelper>;
    friend ::djinni::JniInterface<::testsuite::FunctionPrologueHelper, NativeFunctionPrologueHelper>;

};

}  // namespace djinni_generated

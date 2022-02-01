// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from ident_explicit.djinni

#include "NativeTestIdentRecord.hpp"  // my header
#include "Marshal.hpp"

namespace djinni_generated {

NativeTestIdentRecord::NativeTestIdentRecord() = default;

NativeTestIdentRecord::~NativeTestIdentRecord() = default;

auto NativeTestIdentRecord::fromCpp(JNIEnv* jniEnv, const CppType& c) -> ::djinni::LocalRef<JniType> {
    const auto& data = ::djinni::JniClass<NativeTestIdentRecord>::get();
    auto r = ::djinni::LocalRef<JniType>{jniEnv->NewObject(data.clazz.get(), data.jconstructor,
                                                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c.FirstValue)),
                                                           ::djinni::get(::djinni::String::fromCpp(jniEnv, c.second_value)))};
    ::djinni::jniExceptionCheck(jniEnv);
    return r;
}

auto NativeTestIdentRecord::toCpp(JNIEnv* jniEnv, JniType j) -> CppType {
    ::djinni::JniLocalScope jscope(jniEnv, 3);
    assert(j != nullptr);
    const auto& data = ::djinni::JniClass<NativeTestIdentRecord>::get();
    return {::djinni::I32::toCpp(jniEnv, jniEnv->GetIntField(j, data.field_mFirstValue)),
            ::djinni::String::toCpp(jniEnv, (jstring)jniEnv->GetObjectField(j, data.field_mSecondValue))};
}

}  // namespace djinni_generated
// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from client_interface.djinni

#pragma once

#include "client_returned_record.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class NativeClientReturnedRecord final {
public:
    using CppType = ::testsuite::ClientReturnedRecord;
    using JniType = jobject;

    using Boxed = NativeClientReturnedRecord;

    ~NativeClientReturnedRecord();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    NativeClientReturnedRecord();
    friend ::djinni::JniClass<NativeClientReturnedRecord>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/dropbox/djinni/test/ClientReturnedRecord") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(JLjava/lang/String;Ljava/lang/String;)V") };
    const jfieldID field_mRecordId { ::djinni::jniGetFieldID(clazz.get(), "mRecordId", "J") };
    const jfieldID field_mContent { ::djinni::jniGetFieldID(clazz.get(), "mContent", "Ljava/lang/String;") };
    const jfieldID field_mMisc { ::djinni::jniGetFieldID(clazz.get(), "mMisc", "Ljava/lang/String;") };
};

} // namespace djinni_generated

// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from wchar_test.djinni

#include "NativeWcharTestHelpers.hpp"  // my header
#include "Marshal.hpp"
#include "NativeWcharTestRec.hpp"

namespace djinni_generated {

NativeWcharTestHelpers::NativeWcharTestHelpers() : ::djinni::JniInterface<::testsuite::WcharTestHelpers, NativeWcharTestHelpers>("com/dropbox/djinni/test/WcharTestHelpers$CppProxy") {}

NativeWcharTestHelpers::~NativeWcharTestHelpers() = default;


CJNIEXPORT void JNICALL Java_com_dropbox_djinni_test_WcharTestHelpers_00024CppProxy_nativeDestroy(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef)
{
    try {
        delete reinterpret_cast<::djinni::CppProxyHandle<::testsuite::WcharTestHelpers>*>(nativeRef);
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_WcharTestHelpers_getRecord(JNIEnv* jniEnv, jobject /*this*/)
{
    try {
        auto r = ::testsuite::WcharTestHelpers::get_record();
        return ::djinni::release(::djinni_generated::NativeWcharTestRec::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jstring JNICALL Java_com_dropbox_djinni_test_WcharTestHelpers_getString(JNIEnv* jniEnv, jobject /*this*/)
{
    try {
        auto r = ::testsuite::WcharTestHelpers::get_string();
        return ::djinni::release(::djinni::WString::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jboolean JNICALL Java_com_dropbox_djinni_test_WcharTestHelpers_checkString(JNIEnv* jniEnv, jobject /*this*/, jstring j_str)
{
    try {
        auto r = ::testsuite::WcharTestHelpers::check_string(::djinni::WString::toCpp(jniEnv, j_str));
        return ::djinni::release(::djinni::Bool::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jboolean JNICALL Java_com_dropbox_djinni_test_WcharTestHelpers_checkRecord(JNIEnv* jniEnv, jobject /*this*/, jobject j_rec)
{
    try {
        auto r = ::testsuite::WcharTestHelpers::check_record(::djinni_generated::NativeWcharTestRec::toCpp(jniEnv, j_rec));
        return ::djinni::release(::djinni::Bool::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

}  // namespace djinni_generated

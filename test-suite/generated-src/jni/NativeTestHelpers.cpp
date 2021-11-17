// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from test.djinni

#include "NativeTestHelpers.hpp"  // my header
#include "Marshal.hpp"
#include "NativeAssortedPrimitives.hpp"
#include "NativeClientInterface.hpp"
#include "NativeColor.hpp"
#include "NativeMapListRecord.hpp"
#include "NativeNestedCollection.hpp"
#include "NativePrimitiveList.hpp"
#include "NativeSetRecord.hpp"
#include "NativeUserToken.hpp"

namespace djinni_generated {

NativeTestHelpers::NativeTestHelpers() : ::djinni::JniInterface<::testsuite::TestHelpers, NativeTestHelpers>("com/dropbox/djinni/test/TestHelpers$CppProxy") {}

NativeTestHelpers::~NativeTestHelpers() = default;


CJNIEXPORT void JNICALL Java_com_dropbox_djinni_test_TestHelpers_00024CppProxy_nativeDestroy(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef)
{
    try {
        delete reinterpret_cast<::djinni::CppProxyHandle<::testsuite::TestHelpers>*>(nativeRef);
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_TestHelpers_getSetRecord(JNIEnv* jniEnv, jobject /*this*/)
{
    try {
        auto r = ::testsuite::TestHelpers::get_set_record();
        return ::djinni::release(::djinni_generated::NativeSetRecord::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jboolean JNICALL Java_com_dropbox_djinni_test_TestHelpers_checkSetRecord(JNIEnv* jniEnv, jobject /*this*/, jobject j_rec)
{
    try {
        auto r = ::testsuite::TestHelpers::check_set_record(::djinni_generated::NativeSetRecord::toCpp(jniEnv, j_rec));
        return ::djinni::release(::djinni::Bool::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_TestHelpers_getPrimitiveList(JNIEnv* jniEnv, jobject /*this*/)
{
    try {
        auto r = ::testsuite::TestHelpers::get_primitive_list();
        return ::djinni::release(::djinni_generated::NativePrimitiveList::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jboolean JNICALL Java_com_dropbox_djinni_test_TestHelpers_checkPrimitiveList(JNIEnv* jniEnv, jobject /*this*/, jobject j_pl)
{
    try {
        auto r = ::testsuite::TestHelpers::check_primitive_list(::djinni_generated::NativePrimitiveList::toCpp(jniEnv, j_pl));
        return ::djinni::release(::djinni::Bool::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_TestHelpers_getNestedCollection(JNIEnv* jniEnv, jobject /*this*/)
{
    try {
        auto r = ::testsuite::TestHelpers::get_nested_collection();
        return ::djinni::release(::djinni_generated::NativeNestedCollection::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jboolean JNICALL Java_com_dropbox_djinni_test_TestHelpers_checkNestedCollection(JNIEnv* jniEnv, jobject /*this*/, jobject j_nc)
{
    try {
        auto r = ::testsuite::TestHelpers::check_nested_collection(::djinni_generated::NativeNestedCollection::toCpp(jniEnv, j_nc));
        return ::djinni::release(::djinni::Bool::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_TestHelpers_getMap(JNIEnv* jniEnv, jobject /*this*/)
{
    try {
        auto r = ::testsuite::TestHelpers::get_map();
        return ::djinni::release(::djinni::Map<::djinni::String, ::djinni::I64>::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jboolean JNICALL Java_com_dropbox_djinni_test_TestHelpers_checkMap(JNIEnv* jniEnv, jobject /*this*/, jobject j_m)
{
    try {
        auto r = ::testsuite::TestHelpers::check_map(::djinni::Map<::djinni::String, ::djinni::I64>::toCpp(jniEnv, j_m));
        return ::djinni::release(::djinni::Bool::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_TestHelpers_getEmptyMap(JNIEnv* jniEnv, jobject /*this*/)
{
    try {
        auto r = ::testsuite::TestHelpers::get_empty_map();
        return ::djinni::release(::djinni::Map<::djinni::String, ::djinni::I64>::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jboolean JNICALL Java_com_dropbox_djinni_test_TestHelpers_checkEmptyMap(JNIEnv* jniEnv, jobject /*this*/, jobject j_m)
{
    try {
        auto r = ::testsuite::TestHelpers::check_empty_map(::djinni::Map<::djinni::String, ::djinni::I64>::toCpp(jniEnv, j_m));
        return ::djinni::release(::djinni::Bool::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_TestHelpers_getMapListRecord(JNIEnv* jniEnv, jobject /*this*/)
{
    try {
        auto r = ::testsuite::TestHelpers::get_map_list_record();
        return ::djinni::release(::djinni_generated::NativeMapListRecord::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jboolean JNICALL Java_com_dropbox_djinni_test_TestHelpers_checkMapListRecord(JNIEnv* jniEnv, jobject /*this*/, jobject j_m)
{
    try {
        auto r = ::testsuite::TestHelpers::check_map_list_record(::djinni_generated::NativeMapListRecord::toCpp(jniEnv, j_m));
        return ::djinni::release(::djinni::Bool::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT void JNICALL Java_com_dropbox_djinni_test_TestHelpers_checkClientInterfaceAscii(JNIEnv* jniEnv, jobject /*this*/, jobject j_i)
{
    try {
        ::testsuite::TestHelpers::check_client_interface_ascii(::djinni_generated::NativeClientInterface::toCpp(jniEnv, j_i));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_com_dropbox_djinni_test_TestHelpers_checkClientInterfaceNonascii(JNIEnv* jniEnv, jobject /*this*/, jobject j_i)
{
    try {
        ::testsuite::TestHelpers::check_client_interface_nonascii(::djinni_generated::NativeClientInterface::toCpp(jniEnv, j_i));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_com_dropbox_djinni_test_TestHelpers_checkClientInterfaceArgs(JNIEnv* jniEnv, jobject /*this*/, jobject j_i)
{
    try {
        ::testsuite::TestHelpers::check_client_interface_args(::djinni_generated::NativeClientInterface::toCpp(jniEnv, j_i));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_com_dropbox_djinni_test_TestHelpers_checkEnumMap(JNIEnv* jniEnv, jobject /*this*/, jobject j_m)
{
    try {
        ::testsuite::TestHelpers::check_enum_map(::djinni::Map<::djinni_generated::NativeColor, ::djinni::String>::toCpp(jniEnv, j_m));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_com_dropbox_djinni_test_TestHelpers_checkEnum(JNIEnv* jniEnv, jobject /*this*/, jobject j_c)
{
    try {
        ::testsuite::TestHelpers::check_enum(::djinni_generated::NativeColor::toCpp(jniEnv, j_c));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_TestHelpers_tokenId(JNIEnv* jniEnv, jobject /*this*/, jobject j_t)
{
    try {
        auto r = ::testsuite::TestHelpers::token_id(::djinni_generated::NativeUserToken::toCpp(jniEnv, j_t));
        return ::djinni::release(::djinni_generated::NativeUserToken::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_TestHelpers_createCppToken(JNIEnv* jniEnv, jobject /*this*/)
{
    try {
        auto r = ::testsuite::TestHelpers::create_cpp_token();
        return ::djinni::release(::djinni_generated::NativeUserToken::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT void JNICALL Java_com_dropbox_djinni_test_TestHelpers_checkCppToken(JNIEnv* jniEnv, jobject /*this*/, jobject j_t)
{
    try {
        ::testsuite::TestHelpers::check_cpp_token(::djinni_generated::NativeUserToken::toCpp(jniEnv, j_t));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT jlong JNICALL Java_com_dropbox_djinni_test_TestHelpers_cppTokenId(JNIEnv* jniEnv, jobject /*this*/, jobject j_t)
{
    try {
        auto r = ::testsuite::TestHelpers::cpp_token_id(::djinni_generated::NativeUserToken::toCpp(jniEnv, j_t));
        return ::djinni::release(::djinni::I64::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT void JNICALL Java_com_dropbox_djinni_test_TestHelpers_checkTokenType(JNIEnv* jniEnv, jobject /*this*/, jobject j_t, jstring j_type)
{
    try {
        ::testsuite::TestHelpers::check_token_type(::djinni_generated::NativeUserToken::toCpp(jniEnv, j_t),
                                                   ::djinni::String::toCpp(jniEnv, j_type));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_TestHelpers_returnNone(JNIEnv* jniEnv, jobject /*this*/)
{
    try {
        auto r = ::testsuite::TestHelpers::return_none();
        return ::djinni::release(::djinni::Optional<std::experimental::optional, ::djinni::I32>::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_TestHelpers_assortedPrimitivesId(JNIEnv* jniEnv, jobject /*this*/, jobject j_i)
{
    try {
        auto r = ::testsuite::TestHelpers::assorted_primitives_id(::djinni_generated::NativeAssortedPrimitives::toCpp(jniEnv, j_i));
        return ::djinni::release(::djinni_generated::NativeAssortedPrimitives::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jbyteArray JNICALL Java_com_dropbox_djinni_test_TestHelpers_idBinary(JNIEnv* jniEnv, jobject /*this*/, jbyteArray j_b)
{
    try {
        auto r = ::testsuite::TestHelpers::id_binary(::djinni::Binary::toCpp(jniEnv, j_b));
        return ::djinni::release(::djinni::Binary::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_TestHelpers_getAsyncResult(JNIEnv* jniEnv, jobject /*this*/)
{
    try {
        auto r = ::testsuite::TestHelpers::get_async_result();
        return ::djinni::release(::djinni::FutureAdaptor<::djinni::I32>::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_TestHelpers_futureRoundtrip(JNIEnv* jniEnv, jobject /*this*/, jobject f)
{
    try {
        auto r = ::testsuite::TestHelpers::future_roundtrip(::djinni::FutureAdaptor<::djinni::I32>::toCpp(jniEnv, f));
        return ::djinni::release(::djinni::FutureAdaptor<::djinni::String>::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

}  // namespace djinni_generated

// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from single_language_interfaces.djinni

#include "NativeUsesSingleLanguageListeners.hpp"  // my header
#include "NativeJavaOnlyListener.hpp"
#include "NativeObjcOnlyListener.hpp"

namespace djinni_generated {

NativeUsesSingleLanguageListeners::NativeUsesSingleLanguageListeners() : ::djinni::JniInterface<::testsuite::UsesSingleLanguageListeners, NativeUsesSingleLanguageListeners>("com/dropbox/djinni/test/UsesSingleLanguageListeners$CppProxy") {}

NativeUsesSingleLanguageListeners::~NativeUsesSingleLanguageListeners() = default;

NativeUsesSingleLanguageListeners::JavaProxy::JavaProxy(JniType j) : Handle(::djinni::jniGetThreadEnv(), j) { }

NativeUsesSingleLanguageListeners::JavaProxy::~JavaProxy() = default;

void NativeUsesSingleLanguageListeners::JavaProxy::callForObjC(const /*not-null*/ std::shared_ptr<::testsuite::ObjcOnlyListener> & c_l) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeUsesSingleLanguageListeners>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_callForObjC,
                           ::djinni::get(::djinni_generated::NativeObjcOnlyListener::fromCpp(jniEnv, c_l)));
    ::djinni::jniExceptionCheck(jniEnv);
}
/*not-null*/ std::shared_ptr<::testsuite::ObjcOnlyListener> NativeUsesSingleLanguageListeners::JavaProxy::returnForObjC() {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeUsesSingleLanguageListeners>::get();
    auto jret = jniEnv->CallObjectMethod(Handle::get().get(), data.method_returnForObjC);
    ::djinni::jniExceptionCheck(jniEnv);
    return ::djinni_generated::NativeObjcOnlyListener::toCpp(jniEnv, jret);
}
void NativeUsesSingleLanguageListeners::JavaProxy::callForJava(const /*not-null*/ std::shared_ptr<::testsuite::JavaOnlyListener> & c_l) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeUsesSingleLanguageListeners>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_callForJava,
                           ::djinni::get(::djinni_generated::NativeJavaOnlyListener::fromCpp(jniEnv, c_l)));
    ::djinni::jniExceptionCheck(jniEnv);
}
/*not-null*/ std::shared_ptr<::testsuite::JavaOnlyListener> NativeUsesSingleLanguageListeners::JavaProxy::returnForJava() {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeUsesSingleLanguageListeners>::get();
    auto jret = jniEnv->CallObjectMethod(Handle::get().get(), data.method_returnForJava);
    ::djinni::jniExceptionCheck(jniEnv);
    return ::djinni_generated::NativeJavaOnlyListener::toCpp(jniEnv, jret);
}

CJNIEXPORT void JNICALL Java_com_dropbox_djinni_test_UsesSingleLanguageListeners_00024CppProxy_nativeDestroy(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef)
{
    try {
        delete reinterpret_cast<::djinni::CppProxyHandle<::testsuite::UsesSingleLanguageListeners>*>(nativeRef);
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_com_dropbox_djinni_test_UsesSingleLanguageListeners_00024CppProxy_native_1callForObjC(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, jobject j_l)
{
    try {
        const auto& ref = ::djinni::objectFromHandleAddress<::testsuite::UsesSingleLanguageListeners>(nativeRef);
        ref->callForObjC(::djinni_generated::NativeObjcOnlyListener::toCpp(jniEnv, j_l));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_UsesSingleLanguageListeners_00024CppProxy_native_1returnForObjC(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef)
{
    try {
        const auto& ref = ::djinni::objectFromHandleAddress<::testsuite::UsesSingleLanguageListeners>(nativeRef);
        auto r = ref->returnForObjC();
        return ::djinni::release(::djinni_generated::NativeObjcOnlyListener::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT void JNICALL Java_com_dropbox_djinni_test_UsesSingleLanguageListeners_00024CppProxy_native_1callForJava(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, jobject j_l)
{
    try {
        const auto& ref = ::djinni::objectFromHandleAddress<::testsuite::UsesSingleLanguageListeners>(nativeRef);
        ref->callForJava(::djinni_generated::NativeJavaOnlyListener::toCpp(jniEnv, j_l));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT jobject JNICALL Java_com_dropbox_djinni_test_UsesSingleLanguageListeners_00024CppProxy_native_1returnForJava(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef)
{
    try {
        const auto& ref = ::djinni::objectFromHandleAddress<::testsuite::UsesSingleLanguageListeners>(nativeRef);
        auto r = ref->returnForJava();
        return ::djinni::release(::djinni_generated::NativeJavaOnlyListener::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

}  // namespace djinni_generated

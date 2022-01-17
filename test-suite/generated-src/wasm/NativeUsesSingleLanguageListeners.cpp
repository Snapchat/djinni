// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from single_language_interfaces.djinni

#include "NativeUsesSingleLanguageListeners.hpp"  // my header
#include "NativeJavaOnlyListener.hpp"
#include "NativeObjcOnlyListener.hpp"

namespace djinni_generated {

em::val NativeUsesSingleLanguageListeners::cppProxyMethods() {
    static const em::val methods = em::val::array(std::vector<std::string> {
        "callForObjC",
        "returnForObjC",
        "callForJava",
        "returnForJava",
    });
    return methods;
}

void NativeUsesSingleLanguageListeners::callForObjC(const CppType& self, const em::val& w_l) {
    self->callForObjC(::djinni_generated::NativeObjcOnlyListener::toCpp(w_l));
}
em::val NativeUsesSingleLanguageListeners::returnForObjC(const CppType& self) {
    auto r = self->returnForObjC();
    return ::djinni_generated::NativeObjcOnlyListener::fromCpp(r);
}
void NativeUsesSingleLanguageListeners::callForJava(const CppType& self, const em::val& w_l) {
    self->callForJava(::djinni_generated::NativeJavaOnlyListener::toCpp(w_l));
}
em::val NativeUsesSingleLanguageListeners::returnForJava(const CppType& self) {
    auto r = self->returnForJava();
    return ::djinni_generated::NativeJavaOnlyListener::fromCpp(r);
}

void NativeUsesSingleLanguageListeners::JsProxy::callForObjC(const std::shared_ptr<::testsuite::ObjcOnlyListener> & l) {
    auto ret = callMethod("callForObjC", ::djinni_generated::NativeObjcOnlyListener::fromCpp(l));
    checkError(ret);
}

std::shared_ptr<::testsuite::ObjcOnlyListener> NativeUsesSingleLanguageListeners::JsProxy::returnForObjC() {
    auto ret = callMethod("returnForObjC");
    checkError(ret);
    return ::djinni_generated::NativeObjcOnlyListener::toCpp(ret);
}

void NativeUsesSingleLanguageListeners::JsProxy::callForJava(const std::shared_ptr<::testsuite::JavaOnlyListener> & l) {
    auto ret = callMethod("callForJava", ::djinni_generated::NativeJavaOnlyListener::fromCpp(l));
    checkError(ret);
}

std::shared_ptr<::testsuite::JavaOnlyListener> NativeUsesSingleLanguageListeners::JsProxy::returnForJava() {
    auto ret = callMethod("returnForJava");
    checkError(ret);
    return ::djinni_generated::NativeJavaOnlyListener::toCpp(ret);
}

namespace {
    EM_JS(void, djinni_init_testsuite_uses_single_language_listeners, (), {
        'testsuite'.split('.').reduce(function(path, part) {
            if (!(part in path)) { path[part] = {}}; 
            return path[part]
        }, Module);
        Module.testsuite.UsesSingleLanguageListeners = Module.testsuite_UsesSingleLanguageListeners
    })
}
void NativeUsesSingleLanguageListeners::staticInitialize() {
    static std::once_flag initOnce;
    std::call_once(initOnce, djinni_init_testsuite_uses_single_language_listeners);
}
EMSCRIPTEN_BINDINGS(testsuite_uses_single_language_listeners) {
    em::class_<::testsuite::UsesSingleLanguageListeners>("testsuite_UsesSingleLanguageListeners")
        .smart_ptr<std::shared_ptr<::testsuite::UsesSingleLanguageListeners>>("testsuite_UsesSingleLanguageListeners")
        .function("nativeDestroy", &NativeUsesSingleLanguageListeners::nativeDestroy)
        .function("callForObjC", NativeUsesSingleLanguageListeners::callForObjC)
        .function("returnForObjC", NativeUsesSingleLanguageListeners::returnForObjC)
        .function("callForJava", NativeUsesSingleLanguageListeners::callForJava)
        .function("returnForJava", NativeUsesSingleLanguageListeners::returnForJava)
        ;
    NativeUsesSingleLanguageListeners::staticInitialize();
}

}  // namespace djinni_generated

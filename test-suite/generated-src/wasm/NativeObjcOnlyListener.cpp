// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from single_language_interfaces.djinni

#include "NativeObjcOnlyListener.hpp"  // my header

namespace djinni_generated {


namespace {
    EM_JS(void, djinni_init_testsuite_objc_only_listener, (), {
        'testsuite'.split('.').reduce(function(path, part) {
            if (!(part in path)) { path[part] = {}}; 
            return path[part]}, Module);
        Module.testsuite.ObjcOnlyListener = Module.testsuite_ObjcOnlyListener
    })
}
void NativeObjcOnlyListener::staticInitialize() {
    static std::once_flag initOnce;
    std::call_once(initOnce, djinni_init_testsuite_objc_only_listener);
}
EMSCRIPTEN_BINDINGS(testsuite_objc_only_listener) {
    em::class_<::testsuite::ObjcOnlyListener>("testsuite_ObjcOnlyListener")
        .smart_ptr<std::shared_ptr<::testsuite::ObjcOnlyListener>>("testsuite_ObjcOnlyListener")
        .function("nativeDestroy", &NativeObjcOnlyListener::nativeDestroy)
        ;
    NativeObjcOnlyListener::staticInitialize();
}

}  // namespace djinni_generated

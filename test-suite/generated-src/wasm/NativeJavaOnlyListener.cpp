// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from single_language_interfaces.djinni

#include "NativeJavaOnlyListener.hpp"  // my header

namespace djinni_generated {


namespace {
    EM_JS(void, djinni_init_testsuite_java_only_listener, (), {
        'testsuite'.split('.').reduce(function(path, part) {
            if (!(part in path)) { path[part] = {}}; 
            return path[part]}, Module);
        Module.testsuite.JavaOnlyListener = Module.testsuite_JavaOnlyListener
    })
}
void NativeJavaOnlyListener::staticInitialize() {
    static std::once_flag initOnce;
    std::call_once(initOnce, djinni_init_testsuite_java_only_listener);
}
EMSCRIPTEN_BINDINGS(testsuite_java_only_listener) {
    em::class_<::testsuite::JavaOnlyListener>("testsuite_JavaOnlyListener")
        .smart_ptr<std::shared_ptr<::testsuite::JavaOnlyListener>>("testsuite_JavaOnlyListener")
        .function("nativeDestroy", &NativeJavaOnlyListener::nativeDestroy)
        ;
    NativeJavaOnlyListener::staticInitialize();
}

}  // namespace djinni_generated

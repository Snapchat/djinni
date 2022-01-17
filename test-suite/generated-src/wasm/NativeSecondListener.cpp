// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from multiple_inheritance.djinni

#include "NativeSecondListener.hpp"  // my header

namespace djinni_generated {


namespace {
    EM_JS(void, djinni_init_testsuite_second_listener, (), {
        'testsuite'.split('.').reduce(function(path, part) {
            if (!(part in path)) { path[part] = {}}; 
            return path[part]
        }, Module);
        Module.testsuite.SecondListener = Module.testsuite_SecondListener
    })
}
void NativeSecondListener::staticInitialize() {
    static std::once_flag initOnce;
    std::call_once(initOnce, djinni_init_testsuite_second_listener);
}
EMSCRIPTEN_BINDINGS(testsuite_second_listener) {
    em::class_<::testsuite::SecondListener>("testsuite_SecondListener")
        .smart_ptr<std::shared_ptr<::testsuite::SecondListener>>("testsuite_SecondListener")
        .function("nativeDestroy", &NativeSecondListener::nativeDestroy)
        ;
    NativeSecondListener::staticInitialize();
}

}  // namespace djinni_generated

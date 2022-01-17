// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from djinni_perf_benchmark.djinni

#include "NativeObjectNative.hpp"  // my header

namespace djinni_generated {

em::val NativeObjectNative::cppProxyMethods() {
    static const em::val methods = em::val::array(std::vector<std::string> {
        "baseline",
    });
    return methods;
}

void NativeObjectNative::baseline(const CppType& self) {
    self->baseline();
}

EMSCRIPTEN_BINDINGS(snapchat_djinni_benchmark_ObjectNative) {
    em::class_<::snapchat::djinni::benchmark::ObjectNative>("ObjectNative")
        .smart_ptr<std::shared_ptr<::snapchat::djinni::benchmark::ObjectNative>>("ObjectNative")
        .function("nativeDestroy", &NativeObjectNative::nativeDestroy)
        .function("baseline", NativeObjectNative::baseline)
        ;
}

}  // namespace djinni_generated

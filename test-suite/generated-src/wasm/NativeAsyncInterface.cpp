// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from test.djinni

#include "NativeAsyncInterface.hpp"  // my header
#include "Future_wasm.hpp"

namespace djinni_generated {


::snapchat::djinni::Future<std::string> NativeAsyncInterface::JsProxy::future_roundtrip(::snapchat::djinni::Future<int32_t> f) {
    auto ret = callMethod("futureRoundtrip", ::snapchat::djinni::FutureAdaptor<::djinni::I32>::fromCpp(f));
    checkError(ret);
    return ::snapchat::djinni::FutureAdaptor<::djinni::String>::toCpp(ret);
}

EMSCRIPTEN_BINDINGS(async_interface) {
    em::class_<::testsuite::AsyncInterface>("AsyncInterface")
        .smart_ptr<std::shared_ptr<::testsuite::AsyncInterface>>("AsyncInterface")
        .function("nativeDestroy", &NativeAsyncInterface::nativeDestroy)
        ;
}

}  // namespace djinni_generated

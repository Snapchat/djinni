// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from test.djinni

#include "NativeAsyncInterface.hpp"  // my header
#include "Future_wasm.hpp"

namespace djinni_generated {


::djinni::Future<std::string> NativeAsyncInterface::JsProxy::future_roundtrip(::djinni::Future<int32_t> f) {
    auto ret = callMethod("futureRoundtrip", ::djinni::FutureAdaptor<::djinni::I32>::fromCpp(std::move(f)));
    checkError(ret);
    return ::djinni::FutureAdaptor<::djinni::String>::toCpp(ret);
}

EMSCRIPTEN_BINDINGS(testsuite_async_interface) {
    ::djinni::DjinniClass_<::testsuite::AsyncInterface>("testsuite_AsyncInterface", "testsuite.AsyncInterface")
        .smart_ptr<std::shared_ptr<::testsuite::AsyncInterface>>("testsuite_AsyncInterface")
        .function("nativeDestroy", &NativeAsyncInterface::nativeDestroy)
        ;
}

} // namespace djinni_generated

// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from yaml-test.djinni

#include "NativeExternInterface2.hpp"  // my header
#include "NativeExternRecordWithDerivings.hpp"
#include "NativeTestHelpers.hpp"

namespace djinni_generated {


::ExternRecordWithDerivings NativeExternInterface2::JsProxy::foo(const std::shared_ptr<::testsuite::TestHelpers> & i) {
    auto ret = callMethod("foo", ::djinni_generated::NativeTestHelpers::fromCpp(i));
    checkError(ret);
    return ::djinni_generated::NativeExternRecordWithDerivings::toCpp(ret);
}

namespace {
    EM_JS(void, djinni_init__extern_interface_2, (), {
        'testsuite'.split('.').reduce(function(path, part) {
            if (!(part in path)) { path[part] = {}}; 
            return path[part]
        }, Module);
        Module.testsuite.ExternInterface2 = Module.testsuite_ExternInterface2
    })
}
void NativeExternInterface2::staticInitialize() {
    static std::once_flag initOnce;
    std::call_once(initOnce, djinni_init__extern_interface_2);
}
EMSCRIPTEN_BINDINGS(_extern_interface_2) {
    em::class_<::ExternInterface2>("testsuite_ExternInterface2")
        .smart_ptr<std::shared_ptr<::ExternInterface2>>("testsuite_ExternInterface2")
        .function("nativeDestroy", &NativeExternInterface2::nativeDestroy)
        ;
    NativeExternInterface2::staticInitialize();
}

}  // namespace djinni_generated

// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from exception.djinni

#include "NativeCppException.hpp"  // my header

namespace djinni_generated {

em::val NativeCppException::cppProxyMethods() {
    static const em::val methods = em::val::array(std::vector<std::string> {
        "throwAnException",
    });
    return methods;
}

int32_t NativeCppException::throw_an_exception(const CppType& self) {
    return ::djinni::I32::fromCpp(self->throw_an_exception());
}
em::val NativeCppException::get() {
    return ::djinni_generated::NativeCppException::fromCpp(::testsuite::CppException::get());
}

EMSCRIPTEN_BINDINGS(cpp_exception) {
    em::class_<::testsuite::CppException>("CppException")
        .smart_ptr<std::shared_ptr<::testsuite::CppException>>("CppException")
        .function("nativeDestroy", &NativeCppException::nativeDestroy)
        .function("throwAnException", NativeCppException::throw_an_exception)
        .class_function("get", NativeCppException::get)
        ;
}

}  // namespace djinni_generated

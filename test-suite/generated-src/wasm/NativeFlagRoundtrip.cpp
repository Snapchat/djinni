// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from enum_flags.djinni

#include "NativeFlagRoundtrip.hpp"  // my header
#include "NativeAccessFlags.hpp"
#include "NativeEmptyFlags.hpp"

namespace djinni_generated {

em::val NativeFlagRoundtrip::cppProxyMethods() {
    static const em::val methods = em::val::array(std::vector<std::string> {
    });
    return methods;
}

int32_t NativeFlagRoundtrip::roundtrip_access(int32_t w_flag) {
    auto r = ::testsuite::FlagRoundtrip::roundtrip_access(::djinni_generated::NativeAccessFlags::toCpp(w_flag));
    return ::djinni_generated::NativeAccessFlags::fromCpp(r);
}
int32_t NativeFlagRoundtrip::roundtrip_empty(int32_t w_flag) {
    auto r = ::testsuite::FlagRoundtrip::roundtrip_empty(::djinni_generated::NativeEmptyFlags::toCpp(w_flag));
    return ::djinni_generated::NativeEmptyFlags::fromCpp(r);
}
em::val NativeFlagRoundtrip::roundtrip_access_boxed(const em::val& w_flag) {
    auto r = ::testsuite::FlagRoundtrip::roundtrip_access_boxed(::djinni::Optional<std::experimental::optional, ::djinni_generated::NativeAccessFlags>::toCpp(w_flag));
    return ::djinni::Optional<std::experimental::optional, ::djinni_generated::NativeAccessFlags>::fromCpp(r);
}
em::val NativeFlagRoundtrip::roundtrip_empty_boxed(const em::val& w_flag) {
    auto r = ::testsuite::FlagRoundtrip::roundtrip_empty_boxed(::djinni::Optional<std::experimental::optional, ::djinni_generated::NativeEmptyFlags>::toCpp(w_flag));
    return ::djinni::Optional<std::experimental::optional, ::djinni_generated::NativeEmptyFlags>::fromCpp(r);
}

EMSCRIPTEN_BINDINGS(testsuite_flag_roundtrip) {
    ::djinni::DjinniClass_<::testsuite::FlagRoundtrip>("testsuite_FlagRoundtrip", "testsuite.FlagRoundtrip")
        .smart_ptr<std::shared_ptr<::testsuite::FlagRoundtrip>>("testsuite_FlagRoundtrip")
        .function("nativeDestroy", &NativeFlagRoundtrip::nativeDestroy)
        .class_function("roundtripAccess", NativeFlagRoundtrip::roundtrip_access)
        .class_function("roundtripEmpty", NativeFlagRoundtrip::roundtrip_empty)
        .class_function("roundtripAccessBoxed", NativeFlagRoundtrip::roundtrip_access_boxed)
        .class_function("roundtripEmptyBoxed", NativeFlagRoundtrip::roundtrip_empty_boxed)
        ;
}

}  // namespace djinni_generated

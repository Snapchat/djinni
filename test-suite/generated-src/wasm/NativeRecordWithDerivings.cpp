// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from derivings.djinni

#include "NativeRecordWithDerivings.hpp"  // my header

namespace djinni_generated {

auto NativeRecordWithDerivings::toCpp(const JsType& j) -> CppType {
    return {::djinni::I8::Boxed::toCpp(j["eight"]),
            ::djinni::I16::Boxed::toCpp(j["sixteen"]),
            ::djinni::I32::Boxed::toCpp(j["thirtytwo"]),
            ::djinni::I64::Boxed::toCpp(j["sixtyfour"]),
            ::djinni::F32::Boxed::toCpp(j["fthirtytwo"]),
            ::djinni::F64::Boxed::toCpp(j["fsixtyfour"]),
            ::djinni::Date::Boxed::toCpp(j["d"]),
            ::djinni::String::Boxed::toCpp(j["s"])};
}
auto NativeRecordWithDerivings::fromCpp(const CppType& c) -> JsType {
    em::val js = em::val::object();
    js.set("eight", ::djinni::I8::Boxed::fromCpp(c.eight));
    js.set("sixteen", ::djinni::I16::Boxed::fromCpp(c.sixteen));
    js.set("thirtytwo", ::djinni::I32::Boxed::fromCpp(c.thirtytwo));
    js.set("sixtyfour", ::djinni::I64::Boxed::fromCpp(c.sixtyfour));
    js.set("fthirtytwo", ::djinni::F32::Boxed::fromCpp(c.fthirtytwo));
    js.set("fsixtyfour", ::djinni::F64::Boxed::fromCpp(c.fsixtyfour));
    js.set("d", ::djinni::Date::Boxed::fromCpp(c.d));
    js.set("s", ::djinni::String::Boxed::fromCpp(c.s));
    return js;
}

} // namespace djinni_generated

// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from wchar_test.djinni

#include "NativeWcharTestRec.hpp"  // my header

namespace djinni_generated {

auto NativeWcharTestRec::toCpp(const JsType& j) -> CppType {
    return {::djinni::WString::Boxed::toCpp(j["s"])};
}
auto NativeWcharTestRec::fromCpp(const CppType& c) -> JsType {
    em::val js = em::val::object();
    js.set("s", ::djinni::WString::Boxed::fromCpp(c.s));
    return js;
}

} // namespace djinni_generated

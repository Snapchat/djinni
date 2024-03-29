// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from client_interface.djinni

#include "NativeClientReturnedRecord.hpp"  // my header

namespace djinni_generated {

auto NativeClientReturnedRecord::toCpp(const JsType& j) -> CppType {
    return {::djinni::I64::Boxed::toCpp(j["recordId"]),
            ::djinni::String::Boxed::toCpp(j["content"]),
            ::djinni::Optional<std::experimental::optional, ::djinni::String>::Boxed::toCpp(j["misc"])};
}
auto NativeClientReturnedRecord::fromCpp(const CppType& c) -> JsType {
    em::val js = em::val::object();
    js.set("recordId", ::djinni::I64::Boxed::fromCpp(c.record_id));
    js.set("content", ::djinni::String::Boxed::fromCpp(c.content));
    js.set("misc", ::djinni::Optional<std::experimental::optional, ::djinni::String>::Boxed::fromCpp(c.misc));
    return js;
}

} // namespace djinni_generated

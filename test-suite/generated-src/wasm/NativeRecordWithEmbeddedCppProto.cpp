// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from proto.djinni

#include "NativeRecordWithEmbeddedCppProto.hpp"  // my header

namespace djinni_generated {

auto NativeRecordWithEmbeddedCppProto::toCpp(const JsType& j) -> CppType {
    return {::djinni::Protobuf<::djinni::test2::PersistingState>::Boxed::toCpp(j["state"])};
}
auto NativeRecordWithEmbeddedCppProto::fromCpp(const CppType& c) -> JsType {
    em::val js = em::val::object();
    js.set("state", ::djinni::Protobuf<::djinni::test2::PersistingState>::Boxed::fromCpp(c.state));
    return js;
}

}  // namespace djinni_generated
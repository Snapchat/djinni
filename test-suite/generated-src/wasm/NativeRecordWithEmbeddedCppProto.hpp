// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from proto.djinni

#pragma once

#include "RecordWithEmbeddedCppProto.hpp"
#include "djinni_wasm.hpp"

namespace djinni_generated {

struct NativeRecordWithEmbeddedCppProto
{
    using CppType = ::testsuite::RecordWithEmbeddedCppProto;
    using JsType = em::val;
    using Boxed = NativeRecordWithEmbeddedCppProto;

    static CppType toCpp(const JsType& j);
    static JsType fromCpp(const CppType& c);
};

} // namespace djinni_generated

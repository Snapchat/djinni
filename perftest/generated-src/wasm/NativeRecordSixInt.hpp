// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from djinni_perf_benchmark.djinni

#pragma once

#include "RecordSixInt.hpp"
#include "djinni_wasm.hpp"

namespace djinni_generated {

struct NativeRecordSixInt
{
    using CppType = ::snapchat::djinni::benchmark::RecordSixInt;
    using JsType = em::val;
    using Boxed = NativeRecordSixInt;

    static CppType toCpp(const JsType& j);
    static JsType fromCpp(const CppType& c);
};

} // namespace djinni_generated

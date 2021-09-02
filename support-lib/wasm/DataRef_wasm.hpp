#pragma once

#include "djinni_wasm.hpp"
#include "DataRef.hpp"

namespace snapchat {
namespace djinni {

struct NativeDataRef {
    using CppType = DataRef;
    using JsType = em::val;

    static CppType toCpp(const JsType& data) {
        return DataRef(data);
    }

    static JsType fromCpp(const CppType& c) {
        return c.platformObj();
    }

    using Boxed = NativeDataRef;
};
}} // namespace snapchat::djinni

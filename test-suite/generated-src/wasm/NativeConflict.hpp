// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from test.djinni

#pragma once

#include "Conflict.hpp"
#include "djinni_wasm.hpp"

namespace djinni_generated {

struct NativeConflict : ::djinni::JsInterface<::testsuite::Conflict, NativeConflict> {
    using CppType = std::shared_ptr<::testsuite::Conflict>;
    using CppOptType = std::shared_ptr<::testsuite::Conflict>;
    using JsType = em::val;
    using Boxed = NativeConflict;

    static CppType toCpp(JsType j) { return _fromJs(j); }
    static JsType fromCppOpt(const CppOptType& c) { return {_toJs(c)}; }
    static JsType fromCpp(const CppType& c) { return fromCppOpt(c); }

    static em::val cppProxyMethods();


    static void staticInitialize();
};

}  // namespace djinni_generated

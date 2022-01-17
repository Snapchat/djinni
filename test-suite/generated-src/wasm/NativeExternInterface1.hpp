// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from yaml-test.djinni

#pragma once

#include "djinni_wasm.hpp"
#include "extern_interface_1.hpp"

namespace djinni_generated {

struct NativeExternInterface1 : ::djinni::JsInterface<::ExternInterface1, NativeExternInterface1> {
    using CppType = std::shared_ptr<::ExternInterface1>;
    using CppOptType = std::shared_ptr<::ExternInterface1>;
    using JsType = em::val;
    using Boxed = NativeExternInterface1;

    static CppType toCpp(JsType j) { return _fromJs(j); }
    static JsType fromCppOpt(const CppOptType& c) { return {_toJs(c)}; }
    static JsType fromCpp(const CppType& c) { return fromCppOpt(c); }

    static em::val cppProxyMethods();

    static em::val foo(const CppType& self, const em::val& w_i);
    static int32_t bar(const CppType& self, int32_t w_e);

    static void staticInitialize();
};

}  // namespace djinni_generated

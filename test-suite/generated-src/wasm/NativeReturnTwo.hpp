// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from multiple_inheritance.djinni

#pragma once

#include "djinni_wasm.hpp"
#include "return_two.hpp"

namespace djinni_generated {

struct NativeReturnTwo : ::djinni::JsInterface<::testsuite::ReturnTwo, NativeReturnTwo> {
    using CppType = std::shared_ptr<::testsuite::ReturnTwo>;
    using CppOptType = std::shared_ptr<::testsuite::ReturnTwo>;
    using JsType = em::val;
    using Boxed = NativeReturnTwo;

    static CppType toCpp(JsType j) { return _fromJs(j); }
    static JsType fromCppOpt(const CppOptType& c) { return {_toJs(c)}; }
    static JsType fromCpp(const CppType& c) {
        djinni::checkForNull(c.get(), "NativeReturnTwo::fromCpp");
        return fromCppOpt(c);
    }

    static em::val cppProxyMethods();

    static em::val get_instance();
    static int8_t return_two(const CppType& self);

};

}  // namespace djinni_generated

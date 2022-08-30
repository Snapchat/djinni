// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from interface_and_abstract_class.djinni

#pragma once

#include "djinni_wasm.hpp"
#include "test_java_interface_only.hpp"

namespace djinni_generated {

struct NativeTestJavaInterfaceOnly : ::djinni::JsInterface<::testsuite::TestJavaInterfaceOnly, NativeTestJavaInterfaceOnly> {
    using CppType = std::shared_ptr<::testsuite::TestJavaInterfaceOnly>;
    using CppOptType = std::shared_ptr<::testsuite::TestJavaInterfaceOnly>;
    using JsType = em::val;
    using Boxed = NativeTestJavaInterfaceOnly;

    static CppType toCpp(JsType j) { return _fromJs(j); }
    static JsType fromCppOpt(const CppOptType& c) { return {_toJs(c)}; }
    static JsType fromCpp(const CppType& c) {
        djinni::checkForNull(c.get(), "NativeTestJavaInterfaceOnly::fromCpp");
        return fromCppOpt(c);
    }


};

}  // namespace djinni_generated
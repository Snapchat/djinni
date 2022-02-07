// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from single_language_interfaces.djinni

#pragma once

#include "djinni_wasm.hpp"
#include "java_only_listener.hpp"

namespace djinni_generated {

struct NativeJavaOnlyListener : ::djinni::JsInterface<::testsuite::JavaOnlyListener, NativeJavaOnlyListener> {
    using CppType = std::shared_ptr<::testsuite::JavaOnlyListener>;
    using CppOptType = std::shared_ptr<::testsuite::JavaOnlyListener>;
    using JsType = em::val;
    using Boxed = NativeJavaOnlyListener;

    static CppType toCpp(JsType j) { return _fromJs(j); }
    static JsType fromCppOpt(const CppOptType& c) { return {_toJs(c)}; }
    static JsType fromCpp(const CppType& c) {
        djinni::checkForNull(c.get(), "NativeJavaOnlyListener::fromCpp");
        return fromCppOpt(c);
    }


};

}  // namespace djinni_generated

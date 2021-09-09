// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from data_ref_view.djinni

#pragma once

#include "DataRefTest.hpp"
#include "djinni_wasm.hpp"

namespace djinni_generated {

struct NativeDataRefTest : ::djinni::JsInterface<::testsuite::DataRefTest, NativeDataRefTest> {
    using CppType = std::shared_ptr<::testsuite::DataRefTest>;
    using CppOptType = std::shared_ptr<::testsuite::DataRefTest>;
    using JsType = em::val;
    using Boxed = NativeDataRefTest;

    static CppType toCpp(JsType j) { return _fromJs(j); }
    static JsType fromCppOpt(const CppOptType& c) { return {_toJs(c)}; }
    static JsType fromCpp(const CppType& c) { return fromCppOpt(c); }

    static em::val cppProxyMethods();

    static void sendData(const CppType& self, const em::val& w_data);
    static em::val retriveAsBin(const CppType& self);
    static void sendMutableData(const CppType& self, const em::val& w_data);
    static em::val generateData(const CppType& self);
    static em::val dataFromVec(const CppType& self);
    static em::val dataFromStr(const CppType& self);
    static em::val sendDataView(const CppType& self, const em::val& w_data);
    static em::val recvDataView(const CppType& self);
    static em::val create();

    struct JsProxy: ::djinni::JsProxyBase, ::testsuite::DataRefTest, ::djinni::InstanceTracker<JsProxy> {
        JsProxy(const em::val& v) : JsProxyBase(v) {}
        void sendData(const ::snapchat::djinni::DataRef & data) override;
        std::vector<uint8_t> retriveAsBin() override;
        void sendMutableData(const ::snapchat::djinni::DataRef & data) override;
        ::snapchat::djinni::DataRef generateData() override;
        ::snapchat::djinni::DataRef dataFromVec() override;
        ::snapchat::djinni::DataRef dataFromStr() override;
        std::vector<uint8_t> sendDataView(const ::snapchat::djinni::DataView & data) override;
        ::snapchat::djinni::DataView recvDataView() override;
    };
};

}  // namespace djinni_generated
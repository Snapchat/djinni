#pragma once

#include "djinni_wasm.hpp"
#include "DataView.hpp"

namespace snapchat {
namespace djinni {

struct NativeDataView {
    using CppType = DataView;
    using JsType = em::val;

    static CppType toCpp(const JsType& o) {
        auto buffer = o["buffer"];
        // check that we are looking at the wasm module's memory space
        assert(buffer == ::djinni::getWasmMemoryBuffer());
        return {reinterpret_cast<uint8_t*>(o["byteOffset"].as<unsigned>()),
            static_cast<size_t>(o["length"].as<unsigned>())};
    }

    static JsType fromCpp(const CppType& c) {
        static auto uint8ArrayClass = em::val::global("Uint8Array");

        unsigned addr = reinterpret_cast<unsigned>(c.buf());
        unsigned size = static_cast<unsigned>(c.len());
        em::val uint8ArrayObj = uint8ArrayClass.new_(::djinni::getWasmMemoryBuffer(), addr, size);

        return uint8ArrayObj;
    }

    using Boxed = NativeDataView;
};

}} // namespace snapchat::djinni

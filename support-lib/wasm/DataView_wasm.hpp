/**
  * Copyright 2021 Snap, Inc.
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *    http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

#pragma once

#include "djinni_wasm.hpp"
#include "../cpp/DataView.hpp"

namespace djinni {

struct NativeDataView {
    using CppType = DataView;
    using JsType = em::val;

    static CppType toCpp(const JsType& o) {
        auto buffer = o["buffer"];
        // check that we are looking at the wasm module's memory space
        assert(buffer == getWasmMemoryBuffer());
        return {reinterpret_cast<uint8_t*>(o["byteOffset"].as<unsigned>()),
            static_cast<size_t>(o["length"].as<unsigned>())};
    }

    static JsType fromCpp(const CppType& c) {
        static auto uint8ArrayClass = em::val::global("Uint8Array");

        unsigned addr = reinterpret_cast<unsigned>(c.buf());
        unsigned size = static_cast<unsigned>(c.len());
        em::val uint8ArrayObj = uint8ArrayClass.new_(getWasmMemoryBuffer(), addr, size);

        return uint8ArrayObj;
    }

    using Boxed = NativeDataView;
};

} // namespace djinni

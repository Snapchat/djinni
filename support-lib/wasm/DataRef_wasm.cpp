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

#include "../cpp/DataRef.hpp"
#include "DataRef_wasm.hpp"

#if DATAREF_WASM

#include "djinni_wasm.hpp"
#include <cassert>

namespace djinni {

class DataRefWasm : public DataRef::Impl {
public:
    // create empty buffer from c++
    explicit DataRefWasm(size_t len) {
        allocate(len);
    }
    // create new data object and initialize with data. although this still
    // copies data, it does the allocation and initialization in one step.
    explicit DataRefWasm(const void* data, size_t len) {
        auto* dbuf = new GenericBuffer<std::vector<uint8_t>>(
            reinterpret_cast<const uint8_t*>(data), len);
        // The js object will destroy the c++ object when its GCed
        _data = dbuf->createJsObject();
    }
    // take over a std::vector's buffer without copying it
    explicit DataRefWasm(std::vector<uint8_t>&& vec) {
        if (!vec.empty()) {
            auto* dbuf = new GenericBuffer<std::vector<uint8_t>>(std::move(vec));
            _data = dbuf->createJsObject();
        } else {
            allocate(0);
        }
    }
    // take over a std::string's buffer without copying it
    explicit DataRefWasm(std::string&& str) {
        if (!str.empty()) {
            auto* dbuf = new GenericBuffer<std::string>(std::move(str));
            _data = dbuf->createJsObject();
        } else {
            allocate(0);
        }
    }
    explicit DataRefWasm(PlatformObject data) {
        auto buffer = data["buffer"];
        assert(buffer == getWasmMemoryBuffer());
        _data = data;
    }
    DataRefWasm(const DataRefWasm&) = delete;

    const uint8_t* buf() const override {
        return reinterpret_cast<const uint8_t*>(_data["byteOffset"].as<unsigned>());
    }
    size_t len() const override {
        return static_cast<size_t>(_data["length"].as<unsigned>());
    }
    uint8_t* mutableBuf() override {
        return reinterpret_cast<uint8_t*>(_data["byteOffset"].as<unsigned>());
    }

    PlatformObject platformObj() const {
        return _data;
    }

private:
    emscripten::val _data = emscripten::val::undefined();

    void allocate(size_t len) {
        auto* dbuf = new GenericBuffer<std::vector<uint8_t>>(len);
        _data = dbuf->createJsObject();
    }
};

DataRef::DataRef(size_t len) {
    _impl = std::make_shared<DataRefWasm>(len);
}

DataRef::DataRef(const void* data, size_t len) {
    _impl = std::make_shared<DataRefWasm>(data, len);
}

DataRef::DataRef(std::vector<uint8_t>&& vec) {
    _impl = std::make_shared<DataRefWasm>(std::move(vec));
}
DataRef::DataRef(std::string&& str) {
    _impl = std::make_shared<DataRefWasm>(std::move(str));
}

DataRef::DataRef(PlatformObject platformObj) {
    _impl = std::make_shared<DataRefWasm>(platformObj);
}

em::val NativeDataRef::fromCpp(const DataRef& c) {
    auto impl = std::dynamic_pointer_cast<DataRefWasm>(c.impl());
    if (!impl) {
        throw std::invalid_argument("DataRef impl is not compatible with WASM");
    }
    return impl->platformObj();
}

} // namespace djinni

#endif

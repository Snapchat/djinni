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

#include "DataRef.hpp"

#if !(DATAREF_JNI || DATAREF_OBJC || DATAREF_WASM)

#include <cassert>

namespace snapchat {
namespace djinni {

class DataRefCpp : public DataRef::Impl {
public:
    explicit DataRefCpp(size_t len) : _storage(len) {}
    DataRefCpp(const DataRefCpp&) = delete;
    explicit DataRefCpp(std::vector<uint8_t>&& vec) : _storage(std::move(vec)) {}

    const uint8_t* buf() const override {
        return _storage.data();
    }
    size_t len() const override {
        return _storage.size();
    }
    uint8_t* mutableBuf() override {
        return _storage.data();
    }

    PlatformObject platformObj() const override {
        return nullptr; // should not be called, as there's no translation
    }

private:
    std::vector<uint8_t> _storage;
};

DataRef::DataRef(size_t len) {
    _impl = std::make_shared<DataRefCpp>(len);
}

DataRef::DataRef(const void* data, size_t len) {
    _impl = std::make_shared<DataRefCpp>(len);
    memcpy(mutableBuf(), data, len);
}

DataRef::DataRef(std::vector<uint8_t>&& vec) {
    _impl = std::make_shared<DataRefCpp>(std::move(vec));
}

DataRef::DataRef(std::string&& str) : DataRef(str.data(), str.size()) {}

}} // namespace snapchat::djinni

#endif

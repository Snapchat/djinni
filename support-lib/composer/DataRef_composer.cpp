/**
  * Copyright 2024 Snap, Inc.
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
#include "DataRef_composer.hpp"

namespace djinni::composer {

struct ComposerDataObject: Composer::ComposerObject {
    COMPOSER_CLASS_HEADER(ComposerDataObject);
    std::variant<std::vector<uint8_t>, std::string, std::shared_ptr<DataRef::Impl>> _data;
};
COMPOSER_CLASS_IMPL(ComposerDataObject);

class DataRefComposer: public DataRef::Impl {
public:
    // create an empty buffer from c++
    explicit DataRefComposer(size_t len) {
        auto bytes = Composer::makeShared<Composer::Bytes>();
        bytes->assignVec(std::vector<uint8_t>(len));
        _array = Composer::makeShared<Composer::ValueTypedArray>(Composer::kDefaultTypedArrayType, bytes);
    }
    // wrap an array object from JS
    explicit DataRefComposer(const Composer::Ref<Composer::ValueTypedArray>& array) {
        _array = array;
    }
    // take over a std::vector's buffer without copying it
    explicit DataRefComposer(std::vector<uint8_t>&& vec) {
        auto container = Composer::makeShared<ComposerDataObject>();
        container->_data = std::move(vec);
        const auto& containedVec = std::get<std::vector<uint8_t>>(container->_data);
        auto bytes = containedVec.data();
        auto len = containedVec.size();
        _array = Composer::makeShared<Composer::ValueTypedArray>(Composer::kDefaultTypedArrayType,
                                                       Composer::BytesView(container, bytes, len));
    }
    // take over a std::string's buffer without copying it
    explicit DataRefComposer(std::string&& str) {
        auto container = Composer::makeShared<ComposerDataObject>();
        container->_data = std::move(str);
        const std::string& containedStr = std::get<std::string>(container->_data);
        auto bytes = reinterpret_cast<const uint8_t*>(containedStr.data());
        auto len = containedStr.size();
        _array = Composer::makeShared<Composer::ValueTypedArray>(Composer::kDefaultTypedArrayType,
                                                                 Composer::BytesView(container, bytes, len));
    }

    DataRefComposer(const DataRefComposer&) = delete;

    const uint8_t* buf() const override {
        return _array->getBuffer().data();
    }
    size_t len() const override {
        return _array->getBuffer().size();
    }
    uint8_t* mutableBuf() override {
        return const_cast<uint8_t*>(_array->getBuffer().data());
    }

    Composer::Ref<Composer::ValueTypedArray> platformObj() const {
        return _array;
    }
    
private:
    Composer::Ref<Composer::ValueTypedArray> _array;
};

DataRef NativeDataRef::toCpp(const Composer::Value& v) {
    auto arr = v.getTypedArrayRef();
    auto impl = std::make_shared<DataRefComposer>(arr);
    return DataRef(impl);
}

Composer::Value NativeDataRef::fromCpp(const DataRef& c) {
    auto impl = std::dynamic_pointer_cast<DataRefComposer>(c.impl());
    if (impl) {
        auto arr = impl->platformObj();
        return Composer::Value(arr);
    } else {
        auto container = Composer::makeShared<ComposerDataObject>();
        auto bytes = c.buf();
        auto len = c.len();
        container->_data = c.impl();
        auto arr = Composer::makeShared<Composer::ValueTypedArray>(Composer::kDefaultTypedArrayType,
                                                                   Composer::BytesView(container, bytes, len));
        return Composer::Value(arr);
    }
}

const Composer::ValueSchema& NativeDataRef::schema() {
    static auto schema = Composer::ValueSchema::valueTypedArray();
    return schema;
}


} // namespace djinni

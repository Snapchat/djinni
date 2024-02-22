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

#include "DataRef_objc.hpp"

#if DATAREF_OBJC

#include <cassert>

namespace djinni {

class DataRefObjc : public DataRef::Impl {
public:
    // create empty buffer from c++
    explicit DataRefObjc(size_t len) {
        allocate(len);
    }
    // create new data object and initialize with data. although this still
    // copies data, it does the allocation and initialization in one step.
    explicit DataRefObjc(const void* data, size_t len) {
        _mutableData = CFDataCreateMutable(kCFAllocatorDefault, len);
        assert(_mutableData != nullptr);
        CFDataAppendBytes(_mutableData, reinterpret_cast<const uint8_t*>(data), len);
        _data = _mutableData;
    }
    // take over a std::vector's buffer without copying it
    explicit DataRefObjc(std::vector<uint8_t>&& vec) {
        if (!vec.empty()) {
            takeOver(std::move(vec));
        } else {
            allocate(0);
        }
    }
    // take over a std::string's buffer without copying it
    explicit DataRefObjc(std::string&& str) {
        if (!str.empty()) {
            takeOver(std::move(str));
        } else {
            allocate(0);
        }
    }
    // wrap an immutable CFData object
    explicit DataRefObjc(CFDataRef data) {
        _data = data;
        _mutableData = nullptr;
        CFRetain(_data);
    }
    // wrap a mutable CFData object (CFMutableData)
    explicit DataRefObjc(CFMutableDataRef data) {
        _data = data;
        _mutableData = data;
        CFRetain(_data);
    }
    DataRefObjc(const DataRefObjc&) = delete;
    ~DataRefObjc() {
        CFRelease(_data);
    }

    const uint8_t* buf() const override {
        // must query from the CF object every time in case the buffer is
        // relocated.
        return CFDataGetBytePtr(_data);
    }
    size_t len() const override {
        // must query from the CF object every time in case the buffer is
        // resized.
        return CFDataGetLength(_data);
    }
    uint8_t* mutableBuf() override {
        return _mutableData ? CFDataGetMutableBytePtr(_mutableData) : nullptr;
    }

    PlatformObject platformObj() const {
        return _data;
    }

private:
    CFDataRef _data;
    CFMutableDataRef _mutableData;

    void allocate(size_t len) {
        _mutableData = CFDataCreateMutable(kCFAllocatorDefault, len);
        assert(_mutableData != nullptr);
        CFDataSetLength(_mutableData, len);
        _data = _mutableData;
    }

    template<typename T>
    void takeOver(T&& obj) {
        using DataObj = std::decay_t<T>;
        CFAllocatorContext context = {};
        // create a new instance and steal the input object's buffer
        auto* p = new DataObj(std::forward<T>(obj));
        auto buf = reinterpret_cast<const uint8_t*>(p->data());
        auto len = p->size();
        context.info = p;
        context.deallocate = [](void* ptr, void* info) {
            // delete the wrapped object
            auto* p = reinterpret_cast<DataObj*>(info);
            delete p;
        };
        CFAllocatorRef deallocator = CFAllocatorCreate(kCFAllocatorDefault, &context);
        assert(deallocator != nullptr);
        // create the CFData without copying
        _data = CFDataCreateWithBytesNoCopy(nullptr, buf, len, deallocator);
        assert(_data != nullptr);
        CFRelease(deallocator); // CFDataCreateWithBytesNoCopy retains the object, so release here
        _mutableData = nullptr; // our CFData is immutable because it can't realloc and resize
    }
};

DataRef::DataRef(size_t len) {
    _impl = std::make_shared<DataRefObjc>(len);
}

DataRef::DataRef(const void* data, size_t len) {
    _impl = std::make_shared<DataRefObjc>(data, len);
}

DataRef::DataRef(std::vector<uint8_t>&& vec) {
    _impl = std::make_shared<DataRefObjc>(std::move(vec));
}
DataRef::DataRef(std::string&& str) {
    _impl = std::make_shared<DataRefObjc>(std::move(str));
}

DataRef::DataRef(CFMutableDataRef platformObj) {
    _impl = std::make_shared<DataRefObjc>(platformObj);
}

DataRef::DataRef(CFDataRef platformObj) {
    _impl = std::make_shared<DataRefObjc>(platformObj);
}

NSData* NativeDataRef::fromCpp(const DataRef& c) {
    auto impl = std::dynamic_pointer_cast<DataRefObjc>(c.impl());
    if (!impl) {
        throw std::invalid_argument("DataRef impl is not compatible with ObjC");
    }
    return (__bridge NSData*)impl->platformObj();
}

} // namespace djinni

#endif

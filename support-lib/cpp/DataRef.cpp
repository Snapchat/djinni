#if !(DATAREF_JNI || DATAREF_OBJC || DATAREF_WASM)

#include "DataRef.hpp"
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

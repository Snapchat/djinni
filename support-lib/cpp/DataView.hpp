#pragma once

#include <cstdint>
#include <functional>
#include <memory>

namespace snapchat {
namespace djinni {

class DataView {
public:
    DataView(uint8_t* p, size_t len) : _buf(p), _len(len) {}
    DataView(const uint8_t* p, size_t len) : _buf(const_cast<uint8_t*>(p)), _len(len) {}

    DataView(DataView&&) = default;

    DataView(const DataView&) = default;

    uint8_t* buf() const {
        return _buf;
    }
    size_t len() const {
        return _len;
    }

private:
    uint8_t* _buf;
    const size_t _len;
};

}} // namespace snapchat::djinni

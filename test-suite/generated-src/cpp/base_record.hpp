// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from optionals.djinni

#pragma once

#include <cstdint>
#include <utility>

namespace testsuite {

/** Base record with a required value */
struct BaseRecord final {
    int32_t val;

    //NOLINTNEXTLINE(google-explicit-constructor)
    BaseRecord(int32_t val_)
    : val(std::move(val_))
    {}
};

} // namespace testsuite
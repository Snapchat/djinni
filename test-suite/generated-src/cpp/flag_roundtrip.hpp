// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from enum_flags.djinni

#pragma once

#include "../../handwritten-src/cpp/optional.hpp"

namespace testsuite {

enum class access_flags : int32_t;
enum class empty_flags : int32_t;

class FlagRoundtrip {
public:
    virtual ~FlagRoundtrip() = default;

    static access_flags roundtrip_access(access_flags flag);

    static empty_flags roundtrip_empty(empty_flags flag);

    static std::experimental::optional<access_flags> roundtrip_access_boxed(std::experimental::optional<access_flags> flag);

    static std::experimental::optional<empty_flags> roundtrip_empty_boxed(std::experimental::optional<empty_flags> flag);
};

} // namespace testsuite

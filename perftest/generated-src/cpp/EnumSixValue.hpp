// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from djinni_perf_benchmark.djinni

#pragma once

#include <functional>

namespace snapchat { namespace djinni { namespace benchmark {

enum class EnumSixValue : int {
    FIRST = 0,
    SECOND = 1,
    THIRD = 2,
    FOURTH = 3,
    FIFTH = 4,
    SIXTH = 5,
};

constexpr const char* to_string(EnumSixValue e) noexcept {
    constexpr const char* names[] = {
        "First",
        "Second",
        "Third",
        "Fourth",
        "Fifth",
        "Sixth",
    };
    return names[static_cast<int>(e)];
}

} } }  // namespace snapchat::djinni::benchmark

namespace std {

template <>
struct hash<::snapchat::djinni::benchmark::EnumSixValue> {
    size_t operator()(::snapchat::djinni::benchmark::EnumSixValue type) const {
        return std::hash<int>()(static_cast<int>(type));
    }
};

}  // namespace std

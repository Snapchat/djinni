// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from example.djinni

#pragma once

#include <functional>

namespace textsort {

enum class sort_order : int {
    ASCENDING = 0,
    DESCENDING = 1,
    RANDOM = 2,
};

constexpr const char* to_string(sort_order e) noexcept {
    constexpr const char* names[] = {
        "ascending",
        "descending",
        "random",
    };
    return names[static_cast<int>(e)];
}

}  // namespace textsort

namespace std {

template <>
struct hash<::textsort::sort_order> {
    size_t operator()(::textsort::sort_order type) const {
        return std::hash<int>()(static_cast<int>(type));
    }
};

}  // namespace std

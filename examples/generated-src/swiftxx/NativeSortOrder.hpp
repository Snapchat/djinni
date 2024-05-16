#pragma once

#include "sort_order.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

struct NativeSortOrder {
    static djinni::AnyValue fromCpp(textsort::sort_order v) {
        return {static_cast<int32_t>(v)};
    }
    static textsort::sort_order toCpp(const djinni::AnyValue& v) {
        auto i = std::get<djinni::I32Value>(v);
        return static_cast<textsort::sort_order>(i);
    }
};

}

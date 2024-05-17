#pragma once

#include "sort_items.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

struct NativeSortItems {
    static djinni::AnyValue fromCpp(const std::shared_ptr<textsort::SortItems>& v) {
        return {v};
    }
    static std::shared_ptr<textsort::SortItems> toCpp(const djinni::AnyValue& v) {
        auto p = std::get<djinni::InterfaceValue>(v);
        return std::reinterpret_pointer_cast<textsort::SortItems>(p);
    }
};

djinni::AnyValue SortItems_createWithListener(const djinni::ParameterList* params);
djinni::AnyValue SortItems_runSort(const djinni::ParameterList* params);
djinni::AnyValue SortItems_sort(const djinni::ParameterList* params);

}

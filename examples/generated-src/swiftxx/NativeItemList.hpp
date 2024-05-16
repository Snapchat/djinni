#pragma once

#include <item_list.hpp>
#include "djinni_support.hpp"

namespace djinni_generated {

struct NativeItemList {
    static djinni::AnyValue fromCpp(const textsort::ItemList& v) {
        auto ret = std::make_shared<djinni::CompositeValue>();
        ret->addValue(djinni::List<djinni::String>::fromCpp(v.items));
        return {ret};
    }
    static textsort::ItemList toCpp(const djinni::AnyValue& v) {
        auto c = std::get<djinni::CompositeValuePtr>(v);
        return textsort::ItemList(djinni::List<djinni::String>::toCpp(c->getValue(0)));
    }
};

}

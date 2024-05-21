#pragma once

#include <item_list.hpp>
#include "djinni_support.hpp"

namespace djinni_generated {

struct NativeItemList {
    static djinni::AnyValue fromCpp(const textsort::ItemList& v) {
        // generate record container
        auto ret = std::make_shared<djinni::CompositeValue>();
        // generate one line for each member of the record
        ret->addValue(djinni::List<djinni::String>::fromCpp(v.items));
        return {ret};
    }
    static textsort::ItemList toCpp(const djinni::AnyValue& v) {
        auto c = std::get<djinni::CompositeValuePtr>(v);
        // generate an argument for each member of the record
        return textsort::ItemList(djinni::List<djinni::String>::toCpp(c->getValue(0)));
    }
};

}

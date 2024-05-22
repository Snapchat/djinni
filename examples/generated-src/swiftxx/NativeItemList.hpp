#pragma once

#include <item_list.hpp>
#include "djinni_support.hpp"

namespace djinni_generated {

struct NativeItemList {
    static djinni::swift::AnyValue fromCpp(const textsort::ItemList& v) {
        // generate record container
        auto ret = std::make_shared<djinni::swift::CompositeValue>();
        // generate one line for each member of the record
        ret->addValue(djinni::swift::List<djinni::swift::String>::fromCpp(v.items));
        return {ret};
    }
    static textsort::ItemList toCpp(const djinni::swift::AnyValue& v) {
        auto c = std::get<djinni::swift::CompositeValuePtr>(v);
        // generate an argument for each member of the record
        return textsort::ItemList(djinni::swift::List<djinni::swift::String>::toCpp(c->getValue(0)));
    }
};

}

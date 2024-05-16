#pragma once

#include "sort_items.hpp"
#include "textbox_listener.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class TextboxListenerProtocolWrapper: public textsort::TextboxListener, public djinni::ProtocolWrapper {
public:
    TextboxListenerProtocolWrapper(void* instance, djinni::DispatchFunc dispatcher): djinni::ProtocolWrapper(instance, dispatcher) {}
    static djinni::AnyValue make(void* instance, djinni::DispatchFunc dispatcher);
    void update(const textsort::ItemList& items) override;
};

djinni::AnyValue SortItems_createWithListener(const djinni::ParameterList& params);

djinni::AnyValue SortItems_runSort(const djinni::ParameterList& params);

djinni::AnyValue SortItems_sort(const djinni::ParameterList& params);

djinni::AnyValue TextboxListener_update(const djinni::ParameterList& params);

}

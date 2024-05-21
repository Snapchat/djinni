#pragma once

#include "textbox_listener.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class TextboxListenerProtocolWrapper: public textsort::TextboxListener, public djinni::ProtocolWrapper {
public:
    TextboxListenerProtocolWrapper(void* instance, djinni::DispatchFunc dispatcher): djinni::ProtocolWrapper(instance, dispatcher) {}
    static djinni::AnyValue make(void* instance, djinni::DispatchFunc dispatcher);
    void update(const textsort::ItemList& items) override;
};

using NativeTextboxListener = djinni::Interface<textsort::TextboxListener>;

djinni::AnyValue TextboxListener_update(const djinni::ParameterList* params);

}

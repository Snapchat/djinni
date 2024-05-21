#pragma once

#include "textbox_listener.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

// generate proxy class for C++ callers
class TextboxListenerProtocolWrapper: public textsort::TextboxListener, public djinni::ProtocolWrapper {
public:
    TextboxListenerProtocolWrapper(void* instance, djinni::DispatchFunc dispatcher): djinni::ProtocolWrapper(instance, dispatcher) {}
    // factory method for Swift caller
    static djinni::AnyValue make(void* instance, djinni::DispatchFunc dispatcher);
    // generate method stubs
    void update(const textsort::ItemList& items) override;
};

using NativeTextboxListener = djinni::Interface<textsort::TextboxListener>;

}

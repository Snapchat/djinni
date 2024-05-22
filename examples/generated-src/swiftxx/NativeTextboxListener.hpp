#pragma once

#include "textbox_listener.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

// generate proxy class for C++ callers
class TextboxListenerProtocolWrapper: public textsort::TextboxListener, public djinni::swift::ProtocolWrapper {
public:
    TextboxListenerProtocolWrapper(void* instance, djinni::swift::DispatchFunc dispatcher): djinni::swift::ProtocolWrapper(instance, dispatcher) {}
    // factory method for Swift caller
    static djinni::swift::AnyValue make(void* instance, djinni::swift::DispatchFunc dispatcher);
    // generate method stubs
    void update(const textsort::ItemList& items) override;
};

using NativeTextboxListener = djinni::swift::Interface<textsort::TextboxListener>;

}

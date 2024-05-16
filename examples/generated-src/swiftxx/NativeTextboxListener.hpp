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

struct NativeTextboxListener {
    static std::shared_ptr<textsort::TextboxListener> toCpp(const djinni::AnyValue& v) {
        auto p = std::get<djinni::InterfaceValue>(v);
        return std::reinterpret_pointer_cast<textsort::TextboxListener>(p);
    }
};

djinni::AnyValue TextboxListener_update(const djinni::ParameterList& params);

}

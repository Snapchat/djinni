#include <NativeTextboxListener.hpp>
#include <NativeItemList.hpp>

namespace djinni_generated {

void TextboxListenerProtocolWrapper::update(const textsort::ItemList& items) {
    djinni::ParameterList params;
    params.addValue(NativeItemList::fromCpp(items));
    callProtocol(0, params);
}

djinni::AnyValue TextboxListener_update(const djinni::ParameterList& params) {
    auto inst = NativeTextboxListener::toCpp(params.getValue(0));
    auto items = NativeItemList::toCpp(params.getValue(1));
    inst->update(items);
    return djinni::nilValue();
}

djinni::AnyValue TextboxListenerProtocolWrapper::make(void* instance, djinni::DispatchFunc dispatcher) {
    return {std::make_shared<TextboxListenerProtocolWrapper>(instance, dispatcher)};
}

}

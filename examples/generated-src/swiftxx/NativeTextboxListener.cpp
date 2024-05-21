#include <NativeTextboxListener.hpp>
#include <NativeItemList.hpp>

namespace djinni_generated {

// +swift method stub
void TextboxListenerProtocolWrapper::update(const textsort::ItemList& items) {
    djinni::ParameterList params;
    params.addValue(NativeItemList::fromCpp(items));
    callProtocol(0, &params);
}

djinni::AnyValue TextboxListenerProtocolWrapper::make(void* instance, djinni::DispatchFunc dispatcher) {
    return {std::make_shared<TextboxListenerProtocolWrapper>(instance, dispatcher)};
}

}

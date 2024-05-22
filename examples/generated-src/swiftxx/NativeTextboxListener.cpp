#include <NativeTextboxListener.hpp>
#include <NativeItemList.hpp>

namespace djinni_generated {

// +swift method stub
void TextboxListenerProtocolWrapper::update(const textsort::ItemList& items) {
    djinni::swift::ParameterList params;
    params.addValue(NativeItemList::fromCpp(items));
    callProtocol(0, &params);
}

djinni::swift::AnyValue TextboxListenerProtocolWrapper::make(void* instance, djinni::swift::DispatchFunc dispatcher) {
    return {std::make_shared<TextboxListenerProtocolWrapper>(instance, dispatcher)};
}

}

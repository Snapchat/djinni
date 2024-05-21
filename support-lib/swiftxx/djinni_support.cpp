#include "djinni_support.hpp"

namespace djinni {

size_t getSize(const AnyValue* v) {
    auto composite = std::get<CompositeValuePtr>(*v);
    return composite->getSize();
}

AnyValue getMember(const AnyValue* v, size_t i) {
    auto composite = std::get<CompositeValuePtr>(*v);
    return composite->getValue(i);
}

AnyValue getMember(const ParameterList* v, size_t i) {
    return v->getValue(i);
}

void addMember(const AnyValue* c, const AnyValue& v) {
    auto composite = std::get<CompositeValuePtr>(*c);
    composite->addValue(v);
}

void setReturnValue(AnyValue* ret, const AnyValue& v) {
    *ret = v;
}

AnyValue makeVoidValue() {
    return VoidValue{};
}

AnyValue makeCompositeValue() {
    return { std::make_shared<CompositeValue>() }; 
}


ProtocolWrapper::ProtocolWrapper(void* ctx, DispatchFunc dispatcher):
    _ctx(ctx), _dispatcher(dispatcher)
{}

ProtocolWrapper::~ProtocolWrapper() {
    _dispatcher(_ctx, -1, nullptr, nullptr);
}

AnyValue ProtocolWrapper::callProtocol(int idx, const ParameterList* params) {
    AnyValue ret = VoidValue();
    _dispatcher(_ctx, idx, params, &ret);
    return ret;
}

}

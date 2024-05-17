#include "djinni_support.hpp"

namespace djinni {

StringValue toString(const AnyValue& v) {
    return std::get<StringValue>(v);
}
AnyValue fromString(const StringValue& s) {
    return {s};
}

I32Value toI32(const AnyValue& v) {
    return std::get<I32Value>(v);
}

AnyValue fromI32(I32Value v) {
    return {v};
}

size_t getSize(const AnyValue& v) {
    auto composite = std::get<CompositeValuePtr>(v);
    return composite->getSize();
}

AnyValue getMember(const AnyValue& v, size_t i) {
    auto composite = std::get<CompositeValuePtr>(v);
    return composite->getValue(i);
}

AnyValue getMember(const ParameterList* v, size_t i) {
    return v->getValue(i);
}

void addMember(const AnyValue& c, const AnyValue& v) {
    auto composite = std::get<CompositeValuePtr>(c);
    composite->addValue(v);
}

void setReturnValue(AnyValue* ret, const AnyValue& v) {
    *ret = v;
}

AnyValue nilValue() {
    return VoidValue{};
}

AnyValue makeCompositeValue() {
    return { std::make_shared<CompositeValue>() }; 
}


ProtocolWrapper::ProtocolWrapper(void* instance, DispatchFunc dispatcher):
    _instance(instance), _dispatcher(dispatcher)
{}

ProtocolWrapper::~ProtocolWrapper() {
    _dispatcher(_instance, -1, nullptr, nullptr);
}

AnyValue ProtocolWrapper::callProtocol(int idx, const ParameterList* params) {
    AnyValue ret = VoidValue();
    _dispatcher(_instance, idx, params, &ret);
    return ret;
}

}

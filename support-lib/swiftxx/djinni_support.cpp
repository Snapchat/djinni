#include "djinni_support.hpp"

namespace djinni::swift {

AnyValue makeRangeValue(void* bytes, size_t size) {
    return RangeValue{bytes, size};
}

RangeValue getRange(const AnyValue& v) {
    return std::get<RangeValue>(v);
}

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

void addMember(AnyValue* c, const AnyValue& v) {
    auto composite = std::get<CompositeValuePtr>(*c);
    composite->addValue(v);
}

void setReturnValue(AnyValue* ret, const AnyValue& v) {
    *ret = v;
}

WeakSwiftProxy weakify(const AnyValue& v) {
    auto i = std::get<InterfaceValue>(v);
    return i.sptr;
}

AnyValue strongify(const WeakSwiftProxy& v) {
    return {v.lock()};
}

InterfaceInfo getInterfaceInfo(const AnyValue* v) {
    auto i = std::get<InterfaceValue>(*v);
    return {i.ptr.get(), i.sptr.get()};
}

AnyValue makeVoidValue() {
    return VoidValue{};
}

bool isVoidValue(const AnyValue* c) {
    return std::holds_alternative<VoidValue>(*c);
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
    AnyValue ret = VoidValue(); // output parameter
    _dispatcher(_ctx, idx, params, &ret);
    return ret;
}

}

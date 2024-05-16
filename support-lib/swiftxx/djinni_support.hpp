#pragma once

#include <variant>
#include <string>
#include <memory>
#include <chrono>
#include <swift/bridging>

namespace djinni {

struct VoidValue{};
using BoolValue = bool;
using I8Value = int8_t;
using I16Value = int16_t;
using I32Value = int32_t;
using I64Value = int64_t;
using FloatValue = float;
using DoubleValue = double;
using StringValue = std::string;
using BinaryValue = std::vector<uint8_t>;
using DateValue = std::chrono::system_clock::time_point;
using EnumValue = int32_t;
using InterfaceValue = std::shared_ptr<void>;

struct CompositeValue;
using CompositeValuePtr = std::shared_ptr<CompositeValue>;

using AnyValue = std::variant<VoidValue,
    BoolValue, I8Value, I16Value, I32Value, I64Value, FloatValue, DoubleValue,
    StringValue, BinaryValue, DateValue, /*EnumValue is identical to I32*/
    InterfaceValue, CompositeValuePtr>;

struct CompositeValue {
    virtual ~CompositeValue() = default;
    std::vector<AnyValue> _elems;

    void addValue(const AnyValue& v) {
        _elems.push_back(v);
    }
    AnyValue getValue(size_t i) const { return _elems[i]; }
    size_t getSize() const { return _elems.size(); }
};

struct ListValue: CompositeValue {};
struct SetValue: CompositeValue {};
struct MapValue: CompositeValue {};
struct OptionalValue: CompositeValue {};
struct RecordValue: CompositeValue {};
struct ParameterList: CompositeValue {};

StringValue toString(const AnyValue& v);
AnyValue fromString(const StringValue& s);
I32Value toI32(const AnyValue& v);
AnyValue fromI32(I32Value v);

size_t getSize(const AnyValue& v);
AnyValue getMember(const AnyValue& v, size_t i);
void addMember(const AnyValue& c, const AnyValue& v);
void setReturnValue(AnyValue* ret, const AnyValue& v);

typedef void (*DispatchFunc)(void* instance, int idx /* -1 for cleanup*/, const ParameterList* params, AnyValue* ret);
class ProtocolWrapper {
    void* _instance;
    DispatchFunc _dispatcher;
protected:
    ProtocolWrapper(void* instance, DispatchFunc dispatcher);
    AnyValue callProtocol(int idx, const ParameterList& params);
public:
    virtual ~ProtocolWrapper();
};

AnyValue nilValue();
AnyValue makeCompositeValue();

struct String {
    using CppType = std::string;
    static AnyValue fromCpp(const CppType& v) {
        return {v};
    }
    static CppType toCpp(const AnyValue& v) {
        return std::get<StringValue>(v);
    }
};

template<typename T>
struct List {
    using CppType = std::vector<typename T::CppType>;
    static AnyValue fromCpp(const CppType& v) {
        auto ret = std::make_shared<CompositeValue>();
        for (const auto& e: v) {
            ret->addValue(T::fromCpp(e));
        }
        return {ret};
    }
    static CppType toCpp(const AnyValue& v) {
        CppType vec;
        auto c = std::get<djinni::CompositeValuePtr>(v);
        for (const auto& item: c->_elems) {
            vec.push_back(T::toCpp(item));
        }
        return vec;
    }
};

}

#pragma once

#include <variant>
#include <string>
#include <memory>
#include <chrono>
#include <unordered_map>

namespace djinni::swift {
// -------- Intermediate data structure and functions to manipulate them from Swift
struct VoidValue{};
// I32 covers bool, i8, i16, i32, enum
using I32Value = int32_t;
using I64Value = int64_t;
// double covers float too
using DoubleValue = double;
using StringValue = std::string;
using BinaryValue = std::vector<uint8_t>;
using DateValue = std::chrono::system_clock::time_point;

class ProtocolWrapper;
struct InterfaceValue {
    std::shared_ptr<void> ptr;
    std::shared_ptr<ProtocolWrapper> sptr;
    template<typename T>
    InterfaceValue(const std::shared_ptr<T>& p) : ptr(p), sptr(std::dynamic_pointer_cast<ProtocolWrapper>(p)) {}
};

struct CompositeValue;
using CompositeValuePtr = std::shared_ptr<CompositeValue>;

using AnyValue = std::variant<VoidValue,
    I32Value, I64Value, DoubleValue,
    StringValue, BinaryValue, DateValue,
    InterfaceValue, CompositeValuePtr>;

struct CompositeValue {
    virtual ~CompositeValue() = default;
    std::vector<AnyValue> _elems;

    void addValue(const AnyValue& v) {
        _elems.push_back(v);
    }
    void addValue(AnyValue&& v) {
        _elems.push_back(std::move(v));
    }
    AnyValue getValue(size_t i) const { return _elems[i]; }
    size_t getSize() const { return _elems.size(); }
};

struct ParameterList: CompositeValue {};

size_t getSize(const AnyValue* v);
AnyValue getMember(const AnyValue* v, size_t i);
void addMember(AnyValue* c, const AnyValue& v);
AnyValue getMember(const ParameterList* v, size_t i);
void setReturnValue(AnyValue* ret, const AnyValue& v);
AnyValue makeVoidValue();
AnyValue makeCompositeValue();

struct InterfaceInfo {
    void* cppPointer;
    void* ctxPointer;
};
InterfaceInfo getInterfaceInfo(const AnyValue* v);

using WeakSwiftProxy = std::weak_ptr<ProtocolWrapper>;
WeakSwiftProxy weakify(const AnyValue& v);
AnyValue strongify(const WeakSwiftProxy& v);

// -------- Swift protocol trampoline
typedef void (*DispatchFunc)(void* ctx, int idx /* -1 for cleanup*/, const ParameterList* params, AnyValue* ret);
class ProtocolWrapper {
protected:
    void* _ctx;
    DispatchFunc _dispatcher;
    ProtocolWrapper(void* ctx, DispatchFunc dispatcher);
    AnyValue callProtocol(int idx, const ParameterList* params);
public:
    virtual ~ProtocolWrapper();
    void* ctx() const { return _ctx; }
};

// -------- Built-in marshallers (conversion between C++ types and intermediate types)
template <typename T /*C++ type*/, typename U/*storage type*/>
struct Integer {
    using CppType = T;
    static AnyValue fromCpp(T v) {
        return {static_cast<U>(v)};
    }
    static T toCpp(const AnyValue& v) {
        auto i = std::get<U>(v);
        return static_cast<T>(i);
    }
};

using I32 = Integer<int32_t, I32Value>;

template <typename T>
struct Enum {
    using CppType = T;
    static AnyValue fromCpp(T v) {
        return {static_cast<int32_t>(v)};
    }
    static T toCpp(const AnyValue& v) {
        auto i = std::get<I32Value>(v);
        return static_cast<T>(i);
    }
};

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
        auto c = std::get<CompositeValuePtr>(v);
        for (const auto& item: c->_elems) {
            vec.push_back(T::toCpp(item));
        }
        return vec;
    }
};

template <typename T>
struct Interface {
    using CppType = std::shared_ptr<T>;
    static AnyValue fromCpp(const CppType& v) {
        return {v};
    }
    static CppType toCpp(const AnyValue& v) {
        auto i = std::get<InterfaceValue>(v);
        return std::reinterpret_pointer_cast<T>(i.ptr);
    }
};

// // Keeps track of C++ objects in Swift
// class CppProxyCache {
//     static CppProxyCache& instance();
//     std::unordered_map<uintptr_t/*shared_ptr.data*/, uintptr_t/*unretained swift anyobject pointer*/> _mapPtrToProxy;
// public:
// };

// // Keeps track of Swift objects in C++
// class SwiftProxyCache {
//     static SwiftProxyCache& instance();
//     std::unordered_map<uintptr_t/*shared_ptr.data*/, uintptr_t/*unretained swift anyobject pointer*/> _mapPtrToProxy;
// public:
// };

}

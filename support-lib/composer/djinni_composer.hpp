/**
 * Copyright 2021 Snap, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "composer_core/cpp/Schema/ValueSchema.hpp"
#include "composer_core/cpp/Schema/ValueSchemaRegistry.hpp"
#include "composer_core/cpp/Schema/ValueSchemaTypeResolver.hpp"
#include "composer_core/cpp/Utils/Shared.hpp"
#include "composer_core/cpp/Utils/ValueFunction.hpp"
#include "composer_core/cpp/Utils/ValueFunctionWithCallable.hpp"
#include "composer_core/cpp/Utils/ValueMap.hpp"
#include "composer_core/cpp/Utils/DjinniUtils.hpp"
#include "composer_core/cpp/Utils/ValueTypedObject.hpp"
#include "composer_core/cpp/Utils/ValueTypedProxyObject.hpp"
#include "composer_core/cpp/Utils/ValueTypedArray.hpp"
#include "composer_core/cpp/Utils/StaticString.hpp"
#include "composer_core/ModuleFactory.hpp"

#include <fmt/format.h>

#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>
#include <codecvt>

namespace djinni {

// -------- Helper functions and classes
class JsException : public std::runtime_error {
public:
    // Input must be an instanceof an Error Type
    JsException(Composer::Error e): std::runtime_error(e.toString()), _jsEx(std::move(e))
    {}
    const Composer::Error& cause() const { return _jsEx; }
private:
    Composer::Error _jsEx;
};

template<typename T>
struct ExceptionHandlingTraits {
    static Composer::Value handleNativeException(const std::exception& e, const Composer::ValueFunctionCallContext& callContext) {
        // store C++ exception in JS Error and raise in JS runtime
        auto msg = STRING_FORMAT("C++: {}", e.what());
        callContext.getExceptionTracker().onError(Composer::Error(std::move(msg)));
        return Composer::Value::undefined();
    }
    static Composer::Value handleNativeException(const JsException& e, const Composer::ValueFunctionCallContext& callContext) {
        // JS error passthrough
        callContext.getExceptionTracker().onError(e.cause());
        return Composer::Value::undefined();
    }
};

template<typename T, typename F>
Composer::Value tsFunc(F&& f) {
    return Composer::Value(Composer::makeShared<Composer::ValueFunctionWithCallable>([f = std::forward<F>(f)] (const Composer::ValueFunctionCallContext& callContext) {
        try {
            return f(callContext);
        }
        catch (const JsException& e) {
            return ExceptionHandlingTraits<T>::handleNativeException(e, callContext);
        } catch (const std::exception& e) {
            return ExceptionHandlingTraits<T>::handleNativeException(e, callContext);
        }
        return Composer::Value::undefined();
    }));
}

void checkForNull(void* ptr, const char* context);

Composer::ValueSchema resolveSchema(const Composer::ValueSchema& unresolved, std::function<void()> registerSchemaFunc);

void registerSchemaImpl(const Composer::ValueSchema& schema, bool resolve);

template<typename T>
Composer::ValueSchema getResolvedSchema(const Composer::StringBox& typeName) {
    T::registerSchema(false);
    T::registerSchema(true);
    auto registry = Composer::ValueSchemaRegistry::sharedInstance();
    auto result = registry->getSchemaForTypeName(typeName);
    return result.value();
}

template<class T, class = void>
struct hasSchemaRef : std::false_type { };
template<class T>
struct hasSchemaRef<T, std::void_t<decltype(T::schemaRef)>> : std::true_type { };

template<typename T>
const Composer::ValueSchema& schemaOrRef() {
    if constexpr (hasSchemaRef<T>::value) {
        return T::schemaRef();
    } else {
        return T::schema();
    }
}

// -------- Builtin data type marshallers
template<typename T, typename U = T>
class Primitive {
public:
    using CppType = T;
    using ComposerType = Composer::Value;

    using Boxed = Primitive;

    static CppType toCpp(const ComposerType& v) {
        return static_cast<T>(v.to<U>());
    }
    static ComposerType fromCpp(const CppType& c) {
        return ComposerType(static_cast<U>(c));
    }
    static const Composer::ValueSchema& schema() {
        static auto schema = Composer::ValueSchema::primitiveType<U>();
        return schema;
    }
};

using Bool = Primitive<bool>;
using I8 = Primitive<int8_t, int32_t>;
using I16 = Primitive<int16_t, int32_t>;
using I32 = Primitive<int32_t>;
using I64 = Primitive<int64_t>;
using F32 = Primitive<float, double>;
using F64 = Primitive<double>;
using String = Primitive<std::string>;
template<typename T>
using Enum = Primitive<T, int32_t>;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
class WString {
    using Utf8Converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>;
    using Utf16Converter = std::wstring_convert<std::codecvt_utf16<wchar_t>>;
public:
    using CppType = std::wstring;
    using ComposerType = Composer::Value;

    using Boxed = WString;

    static CppType toCpp(const ComposerType& v) {
        if (v.isInternedString()) {
            auto utf8str = v.toStringBox().toStringView();
            return Utf8Converter{}.from_bytes(utf8str.data(), utf8str.data() + utf8str.size());
        } else {
            const auto* sstr = v.getStaticString();
            if (sstr->encoding() == Composer::StaticString::Encoding::UTF8) {
                return Utf8Converter{}.from_bytes(sstr->utf8Data(), sstr->utf8Data() + sstr->size());
            } else {
                const auto* begin = reinterpret_cast<const char*>(sstr->utf16Data());
                const auto* end = reinterpret_cast<const char*>(sstr->utf16Data() + sstr->size());
                return Utf16Converter{}.from_bytes(begin, end);
            }
        }
    }
    static ComposerType fromCpp(const CppType& c) {
        return ComposerType(Utf8Converter{}.to_bytes(c));
    }
    static const Composer::ValueSchema& schema() {
        static auto schema = Composer::ValueSchema::string();
        return schema;
    }
};
#pragma clang diagnostic pop

class Binary {
public:
    using CppType = std::vector<uint8_t>;
    using ComposerType = Composer::Value;
    using Boxed = Binary;

    static CppType toCpp(const ComposerType& j);
    static ComposerType fromCpp(const CppType& c);
    static const Composer::ValueSchema& schema();
};

class Date {
public:
    using CppType = std::chrono::system_clock::time_point;
    using ComposerType = Composer::Value;
    using Boxed = Date;
    
    static CppType toCpp(const ComposerType& v);
    static ComposerType fromCpp(const CppType& c);
    static const Composer::ValueSchema& schema();
};

template<template<class> class OptionalType, class T>
struct Optional {
    template<typename C>
    static OptionalType<typename C::CppType> opt_type(...);
    template<typename C>
    static typename C::CppOptType opt_type(typename C::CppOptType*);
    using CppType = decltype(opt_type<T>(nullptr));
    using ComposerType = Composer::Value;
    using Boxed = Optional;

    static CppType toCpp(const ComposerType& j) {
        if (j.isUndefined() || j.isNull()) {
            return CppType{};
        } else {
            return T::Boxed::toCpp(j);
        }
    }
    static ComposerType fromCpp(const OptionalType<typename T::CppType>& c) {
        return c ? T::Boxed::fromCpp(*c) : Composer::Value::undefined();
    }
    template<typename C = T>
    static ComposerType fromCpp(const typename C::CppOptType& cppOpt) {
        return T::Boxed::fromCppOpt(cppOpt);
    }
    static const Composer::ValueSchema& schema() {
        static auto schema = schemaOrRef<T>().asOptional();
        return schema;
    }
};

template<typename T>
class List {
    using ECppType = typename T::CppType;
    using EComposerType = typename T::Boxed::ComposerType;

public:
    using CppType = std::vector<ECppType>;
    using ComposerType = Composer::Value;
    using Boxed = List;

    static const Composer::ValueSchema& schema() {
        static auto schema = Composer::ValueSchema::array(schemaOrRef<T>());
        return schema;
    }

    static CppType toCpp(const ComposerType& v) {
        CppType c;
        const auto* a = v.getArray();
        if (a != nullptr) {
            c.reserve(a->size());
            for (size_t i = 0; i < a->size(); ++i) {
                c.push_back(T::Boxed::toCpp((*a)[i]));
            }
        }
        return c;
    }
    static ComposerType fromCpp(const CppType& c) {
        auto newArray = Composer::ValueArray::make(c.size());
        for (size_t i = 0; i < c.size(); ++i) {
            (*newArray)[i] = T::Boxed::fromCpp(c[i]);
        }
        return Composer::Value(newArray);
    }
};

template <typename T>
class Set  {
    using ECppType = typename T::CppType;
    using EComposerType = typename T::Boxed::ComposerType;
public:
    using CppType = std::unordered_set<ECppType>;
    using ComposerType = Composer::Value;
    using Boxed = Set;

    static CppType toCpp(const ComposerType& v) {
        auto es6set = castOrNull<Composer::ES6Set>(v.getComposerObject());
        CppType cppSet;
        for (auto i = es6set->entries.begin(); i != es6set->entries.end(); i++) {
            cppSet.insert(T::toCpp(*i));
        }
        return cppSet;
    }
    static ComposerType fromCpp(const CppType& c) {
        auto es6set = Composer::makeShared<Composer::ES6Set>();
        for (const auto& k: c) {
            es6set->entries.push_back(T::fromCpp(k));
        }
        return Composer::Value(es6set);
    }
    static const Composer::ValueSchema& schema() {
        static auto schema = Composer::ValueSchema::es6set(schemaOrRef<T>());
        return schema;
    }
};

template<typename Key, typename Value>
class Map {
    using CppKeyType = typename Key::CppType;
    using CppValueType = typename Value::CppType;
    using ComposerKeyType = typename Key::Boxed::ComposerType;
    using ComposerValueType = typename Value::Boxed::ComposerType;

public:
    using CppType = std::unordered_map<CppKeyType, CppValueType>;
    using ComposerType = Composer::Value;
    using Boxed = Map;
    static CppType toCpp(const ComposerType& v) {
        auto es6map = castOrNull<Composer::ES6Map>(v.getComposerObject());
        CppType cppMap;
        for (auto i = es6map->entries.begin(); i != es6map->entries.end();) {
            auto k = Key::toCpp(*i++);
            auto v = Value::toCpp(*i++);
            cppMap[k] = std::move(v);
        }
        return cppMap;
    }
    static ComposerType fromCpp(const CppType& c) {
        auto es6Map = Composer::makeShared<Composer::ES6Map>();
        for (const auto& [k, v]: c) {
            es6Map->entries.push_back(Key::fromCpp(k));
            es6Map->entries.push_back(Value::fromCpp(v));
        }
        return Composer::Value(es6Map);
    }
    static const Composer::ValueSchema& schema() {
        static auto schema = Composer::ValueSchema::es6map(schemaOrRef<Key>(), schemaOrRef<Value>());
        return schema;
    }
};

class Void {
public:
    using CppType = void;
    using ComposerType = Composer::Value;
    using Boxed = Void;
    static const Composer::ValueSchema& schema() {
        static auto schema = Composer::ValueSchema::untyped();
        return schema;
    }
};

template<typename CppProto>
class Protobuf {
public:
    using CppType = CppProto;
    using ComposerType = Composer::Value;
    using Boxed = Protobuf;

    static CppType toCpp(ComposerType v)
    {
        return {};
    }
        
    static ComposerType fromCpp(const CppType& c)
    {
        return {};
    }
    static const Composer::ValueSchema& schema() {
        static auto schema = Composer::ValueSchema::untyped();
        return schema;
    }
};

template <typename T>
struct Array {
    using CppType = std::vector<typename T::CppType>;
    using ComposerType = Composer::Value;
    using Boxed = Array;

    static CppType toCpp(const ComposerType& v) {
        return List<T>::toCpp(v);
    }
    static ComposerType fromCpp(const CppType& c) {
        return List<T>::fromCpp(c);
    }
    
    static const Composer::ValueSchema& schema() {
        return List<T>::schema();
    }
};

template <typename T, typename U = Array<T>>
struct PrimitiveArray {
    using CppType = std::vector<typename T::CppType>;
    using ComposerType = Composer::Value;
    using Boxed = PrimitiveArray;
    using CppElemType = typename T::CppType;

    static CppType toCpp(const ComposerType& v) {
        auto arr = v.getTypedArrayRef();
        const auto* bytes = arr->getBuffer().data();
        const auto byteSize = arr->getBuffer().size();
        const CppElemType* typedData = reinterpret_cast<const CppElemType*>(bytes);
        const auto typedSize = byteSize / sizeof(CppElemType);
        return CppType(typedData, typedData + typedSize);
    }
    static ComposerType fromCpp(const CppType& c) {
        auto bytes = Composer::makeShared<Composer::Bytes>();
        bytes->assignData(reinterpret_cast<const Composer::Byte*>(c.data()), c.size() * sizeof(CppElemType));
        auto arr = Composer::makeShared<Composer::ValueTypedArray>(U::getArrayType(), bytes);
        return Composer::Value(arr);
    }
    static const Composer::ValueSchema& schema() {
        static auto schema = Composer::ValueSchema::valueTypedArray();
        return schema;
    }
};
template <>
struct Array<I8> : PrimitiveArray<I8> {
    static Composer::TypedArrayType getArrayType() {
        return Composer::TypedArrayType::Int8Array;
    }
};
template <>
struct Array<I16> : PrimitiveArray<I16> {
    static Composer::TypedArrayType getArrayType() {
        return Composer::TypedArrayType::Int16Array;
    }
};
template <>
struct Array<I32> : PrimitiveArray<I32> {
    static Composer::TypedArrayType getArrayType() {
        return Composer::TypedArrayType::Int32Array;
    }
};
template <>
struct Array<I64> : PrimitiveArray<I64> {
    // Composer::TypedArrayType does not support BigInt64Array
};
template <>
struct Array<F32> : PrimitiveArray<F32> {
    static Composer::TypedArrayType getArrayType() {
        return Composer::TypedArrayType::Float32Array;
    }
};
template <>
struct Array<F64> : PrimitiveArray<F64> {
    static Composer::TypedArrayType getArrayType() {
        return Composer::TypedArrayType::Float64Array;
    }
};

// -------- Interface support
using ComposerProxyId = uint32_t;
struct CppProxyCacheEntry {
    Composer::Weak<Composer::ValueTypedProxyObject> ref;
    int count;
};
class ComposerProxyBase {
protected:
    Composer::Ref<Composer::ValueTypedProxyObject> _js;
    std::vector<Composer::Ref<Composer::ValueFunction>> _methods;

public:
    ComposerProxyBase(Composer::Ref<Composer::ValueTypedProxyObject> js)
        : _js(js), _methods(_js->getTypedObject()->getPropertiesSize()) {}
    virtual ~ComposerProxyBase() = default;
    Composer::Ref<Composer::ValueTypedProxyObject> getProxy() {
        return _js;
    }
    Composer::Value callJsMethod(size_t i, std::initializer_list<Composer::Value> parameters) {
        if (_methods[i] == nullptr) {
            _methods[i] = _js->getTypedObject()->getProperty(i).getFunctionRef();
        }
        constexpr auto flags = static_cast<Composer::ValueFunctionFlags>(Composer::ValueFunctionFlagsCallSync | Composer::ValueFunctionFlagsPropagatesError);
        auto res = _methods[i]->call(flags,  parameters);
        if (res.success()) {
            return res.moveValue();
        } else {
            // Throw JS Error as C++ exception
            throw JsException(res.moveError());
        }
    }
};

extern std::unordered_map<ComposerProxyId, std::weak_ptr<ComposerProxyBase>> jsProxyCache;
extern std::unordered_map<void*, CppProxyCacheEntry> cppProxyCache;
extern std::mutex jsProxyCacheMutex;
extern std::mutex cppProxyCacheMutex;

template<typename T>
class DjinniCppProxyObject : public Composer::ValueTypedProxyObject {
public:
    DjinniCppProxyObject(const Composer::Ref<Composer::ValueTypedObject>& typedObject, const std::shared_ptr<T>& impl)
        : Composer::ValueTypedProxyObject(typedObject), _impl(impl) {}
    ~DjinniCppProxyObject() override {
        std::lock_guard lk(cppProxyCacheMutex);
        auto i = cppProxyCache.find(_impl.get());
        assert(i != cppProxyCache.end());
        if (--(i->second.count) == 0) {
            cppProxyCache.erase(_impl.get());
        }
    }
    std::string_view getType() const final {
        return "Djinni C++ Proxy";
    }
    std::shared_ptr<T> getImpl() const {
        return _impl;
    }

private:
    std::shared_ptr<T> _impl = nullptr;
};

template<typename I, typename Self>
class JsInterface {
public:
    using CppType = std::shared_ptr<I>;
    using CppOptType = std::shared_ptr<I>;
    using ComposerType = Composer::Value;
    using Boxed = Self;

    static CppType toCpp(const ComposerType& v) {
        return _fromJs(v);
    }
    static ComposerType fromCppOpt(const CppOptType& c) {
        return {_toJs(c)};
    }
    static ComposerType fromCpp(const CppType& c) {
        ::djinni::checkForNull(c.get(), typeid(Self).name());
        return fromCppOpt(c);
    }

private:
    template<typename, typename>
    struct GetOrCreateJsProxy {
        std::shared_ptr<I> operator()(const Composer::Value& js) {
            assert(false && "Attempting to pass JS object but interface lacks +p");
            return {};
        }
    };
    template<typename T>
    struct GetOrCreateJsProxy<T, std::void_t<typename T::ComposerProxy>> {
        std::shared_ptr<I> operator()(const Composer::Value& js) {
            auto proxy = js.getTypedProxyObjectRef();
            auto obj = proxy->getTypedObject();
            std::lock_guard lk(jsProxyCacheMutex);
            // check prsence of proxy id in js object
            ComposerProxyId id = proxy->getId();
            auto i = jsProxyCache.find(id);
            if (i != jsProxyCache.end()) {
                auto strongProxyRef = i->second.lock();
                if (strongProxyRef != nullptr) {
                    return std::dynamic_pointer_cast<typename Self::ComposerProxy>(strongProxyRef);
                }
            }
            // not found or cache entry expired
            // create new js proxy and store it in cache
            auto newproxy = std::make_shared<typename Self::ComposerProxy>(proxy);
            jsProxyCache.emplace(id, newproxy);
            return newproxy;
        }
    };

    static std::shared_ptr<I> _fromJs(const Composer::Value& v) {
        // null object
        if (v.isUndefined() || v.isNull()) {
            return {};
        } else if (auto cppproxy = dynamic_cast<DjinniCppProxyObject<I>*>(v.getTypedProxyObjectRef().get());
                   cppproxy != nullptr) {
            // existing cpp proxy
            return cppproxy->getImpl();
        } else {
            return GetOrCreateJsProxy<Self, void>()(v);
        }
    }

    // enable this only when the derived class has `schema` defined
    // (interface +c)
    template<typename, typename>
    struct GetOrCreateCppProxy {
        Composer::Value operator()(const std::shared_ptr<I>& c) {
            assert(false && "Attempting to pass C++ object but interface lacks +c");
            return Composer::Value::undefined();
        }
    };
    template<typename T>
    struct GetOrCreateCppProxy<T, std::void_t<decltype(T::toComposer)>> {
        Composer::Value operator()(const std::shared_ptr<I>& c) {
            // look up in cpp proxy cache
            std::lock_guard lk(cppProxyCacheMutex);
            auto i = cppProxyCache.find(c.get());
            int jsRefCount = 1;
            if (i != cppProxyCache.end()) {
                // found existing cpp proxy
                jsRefCount += i->second.count;
                if (auto strong = Composer::strongRef(i->second.ref); strong != nullptr) {
                    // and it's not expired
                    return Composer::Value(Composer::Ref(strong));
                }
            }
            // not found or cache entry expired
            // create a new cpp proxy and store it in cache
            auto o = Self::toComposer(c);
            if (i == cppProxyCache.end()) {
                cppProxyCache.emplace(c.get(), CppProxyCacheEntry{o.toWeak(), jsRefCount});
            } else {
                i->second = CppProxyCacheEntry{o.toWeak(), jsRefCount};
            }
            return Composer::Value(o);
        }
    };

    static Composer::Value _toJs(const std::shared_ptr<I>& c) {
        if (c == nullptr) {
            // null object
            return Composer::Value::undefined();
        } else if (auto* p = dynamic_cast<ComposerProxyBase*>(c.get())) {
            // unwrap existing js proxy
            return Composer::Value(p->getProxy());
        } else {
            return GetOrCreateCppProxy<Self, void>()(c);
        }
    }
};

} // namespace djinni

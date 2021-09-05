#pragma once

#include <emscripten.h>
#include <emscripten/bind.h>

#include "../expected.hpp"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <optional>
#include <stdexcept>
#include <iostream>

namespace em = emscripten;

namespace djinni {

template<typename T>
class InstanceTracker {
    static int& count() {
        static int theCount = 0;
        return theCount;
    }
public:
    InstanceTracker() {
        // std::cout << "++" << typeid(this).name() << " => " << ++count() << std::endl;
    }
    virtual ~InstanceTracker() {
        // std::cout << "--" << typeid(this).name() << " => " << --count() << std::endl;
    }
};

template<typename T>
class Primitive {
public:
    using CppType = T;
    using JsType = T;

    struct Boxed {
        using JsType = em::val;
        static CppType toCpp(JsType j) {
            return j.as<CppType>();
        }
        static JsType fromCpp(CppType c) {
            return JsType(c);
        }
    };

    static CppType toCpp(const JsType& j) {
        return j;
    }
    static JsType fromCpp(const CppType& c) {
        return c;
    }
};

using Bool = Primitive<bool>;
using I8 = Primitive<int8_t>;
using I16 = Primitive<int16_t>;
using I32 = Primitive<int32_t>;
using I64 = Primitive<int64_t>;
using F32 = Primitive<float>;
using F64 = Primitive<double>;
using String = Primitive<std::string>;
using WString = Primitive<std::wstring>;

template<typename T>
class WasmEnum {
public:
    using CppType = T;
    using JsType = int32_t;

    struct Boxed {
        using JsType = em::val;
        static CppType toCpp(JsType j) {
            return static_cast<CppType>(j.as<int32_t>());
        }
        static JsType fromCpp(CppType c) {
            return JsType(static_cast<int32_t>(c));
        }
    };

    static CppType toCpp(const JsType& j) {
        return static_cast<CppType>(j);
    }
    static JsType fromCpp(const CppType& c) {
        return static_cast<JsType>(c);
    }
};

class Binary {
public:
    using CppType = std::vector<uint8_t>;
    using JsType = em::val;
    using Boxed = Binary;
    
    static CppType toCpp(const JsType& j);
    static JsType fromCpp(const CppType& c);
};

class Date {
public:
    using CppType = std::chrono::system_clock::time_point;
    using JsType = em::val;
    using Boxed = Date;
    
    static CppType toCpp(const JsType& j);
    static JsType fromCpp(const CppType& c);
};

template <template <class> class OptionalType, class T>
struct Optional
{
    template <typename C> static OptionalType<typename C::CppType> opt_type(...);
    template <typename C> static typename C::CppOptType opt_type(typename C::CppOptType *);
    using CppType = decltype(opt_type<T>(nullptr));
    using JsType = em::val;
    using Boxed = Optional;
    
    static CppType toCpp(const JsType& j) {
        if (j.isUndefined() || j.isNull()) {
            return CppType{};
        } else {
            return T::Boxed::toCpp(j);
        }
    }
    static JsType fromCpp(const OptionalType<typename T::CppType>& c) {
        return c ? T::Boxed::fromCpp(*c) : em::val::null();
    }
    template <typename C = T>
    static JsType fromCpp(const typename C::CppOptType& cppOpt) {
        return T::Boxed::fromCppOpt(cppOpt);
    }
};

template <typename T> 
class List {
    using ECppType = typename T::CppType;
    using EJsType = typename T::Boxed::JsType;
public:
    using CppType = std::vector<ECppType>;
    using JsType = em::val;
    using Boxed = List;

    static CppType toCpp(const JsType& j) {
        const size_t l = j["length"].as<size_t>();
        CppType rv;
        rv.reserve(l);
        for (size_t i = 0; i < l; ++i) {
            rv.push_back(T::Boxed::toCpp(j[i]));
        }
        return rv;
    }
    static JsType fromCpp(const CppType& c) {
        em::val new_array = em::val::array();
        for (const auto& e: c) {
            new_array.call<void>("push", T::Boxed::fromCpp(e));
        }
        return new_array;
    }
};

template <typename T>
class Set  {
    using ECppType = typename T::CppType;
    using EJsType = typename T::Boxed::JsType;
public:
    using CppType = std::unordered_set<ECppType>;
    using JsType = em::val;
    using Boxed = Set;

    static CppType toCpp(const JsType& j) {
        static em::val arrayClass = em::val::global("Array");
        em::val entries = arrayClass.call<em::val>("from", j);
        const size_t l = entries["length"].as<size_t>();
        CppType rs;
        for (size_t i = 0; i < l; ++i) {
            rs.insert(T::Boxed::toCpp(entries[i]));
        }
        return rs;
    }
    static JsType fromCpp(const CppType& c) {
        static em::val setClass = em::val::global("Set");
        em::val newSet = setClass.new_();
        for (const auto& e: c) {
            newSet.call<void>("add", T::Boxed::fromCpp(e));
        }
        return newSet;
    }
};

template <typename Key, typename Value>
class Map {
    using CppKeyType = typename Key::CppType;
    using CppValueType = typename Value::CppType;
    using JsKeyType = typename Key::Boxed::JsType;
    using JsValueType = typename Value::Boxed::JsType;
public:
    using CppType = std::unordered_map<CppKeyType, CppValueType>;
    using JsType = em::val;
    using Boxed = Map;

    static CppType toCpp(const JsType& j) {
        static em::val arrayClass = em::val::global("Array");
        em::val entries = arrayClass.call<em::val>("from", j);
        const size_t l = entries["length"].as<size_t>();
        CppType rm;
        for (size_t i = 0; i < l; ++i) {
            rm.emplace(Key::Boxed::toCpp(entries[i][0]),
                       Value::Boxed::toCpp(entries[i][1]));
        }
        return rm;
    }
    static JsType fromCpp(const CppType& c) {
        static em::val mapClass = em::val::global("Map");
        em::val newMap = mapClass.new_();
        for (const auto &[k, v] : c) {
            newMap.call<void>("set", Key::Boxed::fromCpp(k), Value::Boxed::fromCpp(v));
        }
        return newMap;
    }
};

// js success type:
// { result: ...}
// js error type:
// { error: ...}
template <class RESULT, class ERROR>
struct Outcome
{
    using CppType = expected<typename RESULT::CppType, typename ERROR::CppType>;
    using JsType = em::val;
    using Boxed = Outcome;
    
    static CppType toCpp(const JsType& j) {
        em::val res = j["result"];
        if (!res.isUndefined()) {
            return RESULT::Boxed::toCpp(res);
        } else {
            em::val err = j["error"];
            assert(!err.isUndefined());
            return make_unexpected(ERROR::Boxed::toCpp(err));
        }
    }
    static JsType fromCpp(const CppType& c) {
        if (c.has_value()) {
            em::val res = em::val::object();
            res.set("result", RESULT::Boxed::fromCpp(c.value()));
            return res;
        } else {
            em::val err = em::val::object();
            err.set("error", ERROR::Boxed::fromCpp(c.error()));
            return err;
        }
    }
};

template <typename T>
struct Array {
    using CppType = std::vector<typename T::CppType>;
    using JsType = em::val;
    using Boxed = Array;

    static CppType toCpp(const JsType& j) {
        return List<T>::toCpp(j);
    }
    static JsType fromCpp(const CppType& c) {
        return List<T>::fromCpp(c);
    }
};
template <typename T>
struct PrimitiveArray {
    using CppType = std::vector<typename T::CppType>;
    using JsType = em::val;
    using Boxed = PrimitiveArray;

    static CppType toCpp(const JsType& j) {
        static em::val arrayBufferClass = em::val::global("ArrayBuffer");
        assert(j["buffer"].instanceof(arrayBufferClass));
        std::cout << "typed array to vector" << std::endl;
        return em::convertJSArrayToNumberVector<typename T::CppType>(j);
    }
    static JsType fromCpp(const CppType& c) {
        em::val arrayClass = Array<T>::getArrayClass();
        std::cout << "vector to typed array" << std::endl;
        em::val memoryView{ em::typed_memory_view(c.size(), c.data()) };
        em::val buffer = memoryView.call<em::val>("slice", 0);
        return arrayClass.new_(buffer);
    }
};
template <>
struct Array<I8> : PrimitiveArray<I8> {
    static em::val getArrayClass() {
        static em::val arrayClass = em::val::global("Int8Array");
        return arrayClass;
    }
};
template <>
struct Array<I16> : PrimitiveArray<I16> {
    static em::val getArrayClass() {
        static em::val arrayClass = em::val::global("Int16Array");
        return arrayClass;
    }
};
template <>
struct Array<I32> : PrimitiveArray<I32> {
    static em::val getArrayClass() {
        static em::val arrayClass = em::val::global("Int32Array");
        return arrayClass;
    }
};
template <>
struct Array<F32> : PrimitiveArray<F32> {
    static em::val getArrayClass() {
        static em::val arrayClass = em::val::global("Float32Array");
        return arrayClass;
    }
};
template <>
struct Array<F64> : PrimitiveArray<F64> {
    static em::val getArrayClass() {
        static em::val arrayClass = em::val::global("Float64Array");
        return arrayClass;
    }
};

using JsProxyId = uint64_t;

class JsProxyBase {
public:
    JsProxyBase(const em::val& v);
    virtual ~JsProxyBase();
    const em::val& _jsRef() const;
private:
    em::val _js;
    JsProxyId _id;
};

extern JsProxyId nextId;
extern std::unordered_map<JsProxyId, std::weak_ptr<JsProxyBase>> jsProxyCache;
extern std::unordered_map<void*, em::val> cppProxyCache;
extern std::mutex jsProxyCacheMutex;
extern std::mutex cppProxyCacheMutex;

extern em::val getCppProxyFinalizerRegistry();
extern em::val getCppProxyClass();
extern em::val getWasmMemoryBuffer();

template<typename I, typename Self>
struct JsInterface {
    static void nativeDestroy(const std::shared_ptr<I>& cpp) {
        // std::cout << "delete entry from cppProxyCache" << std::endl;
        std::lock_guard lk(cppProxyCacheMutex);
        assert(cppProxyCache.find(cpp.get()) != cppProxyCache.end());
        cppProxyCache.erase(cpp.get());
    }
    static em::val _toJs(const std::shared_ptr<I>& c) {
        if (c == nullptr) {
            // null object
            return em::val::null();
        }
        else if (auto* p = dynamic_cast<JsProxyBase*>(c.get())) {
            // unwrap existing js proxy
            std::cout << "unwrap js object" << std::endl;
            return p->_jsRef();
        } else {
            // look up in cpp proxy cache
            std::lock_guard lk(cppProxyCacheMutex);
            auto i = cppProxyCache.find(c.get());
            if (i != cppProxyCache.end()) {
                // found existing cpp proxy
                // std::cout << "already has cpp proxy" << std::endl;
                auto strongRef = i->second.template call<em::val>("deref");
                if (!strongRef.isUndefined()) {
                    // and it's not expired
                    return strongRef;
                }
            }
            // not found or cache entry expired
            // create a new cpp proxy and store it in cache
            static em::val weakRefClass = em::val::global("WeakRef");
            em::val nativeRef(c);
            em::val cppProxy = getCppProxyClass().new_(nativeRef, Self::cppProxyMethods());
            em::val weakRef = weakRefClass.new_(cppProxy);
            cppProxyCache.emplace(c.get(), weakRef);
            getCppProxyFinalizerRegistry().call<void>("register", cppProxy, nativeRef);
            return cppProxy;
        }
    }
    static std::shared_ptr<I> _fromJs(em::val js) {
        static const em::val nativeRef("_djinni_native_ref");
        // null object
        if (js.isUndefined() || js.isNull()) {
            return {};
        }
        else if (nativeRef.in(js)) {
            // existing cpp proxy
            std::cout << "getting cpp object" << std::endl;
            return js[nativeRef].as<std::shared_ptr<I>>();
        } else {
            std::lock_guard lk(jsProxyCacheMutex);
            // check prsence of proxy id in js object
            JsProxyId id;
            auto idProp = js["_djinni_js_proxy_id"];
            if (idProp.isUndefined()) {
                // no id, assign a new id
                id = nextId++;
                // std::cout << "assign proxy id " << id << std::endl;
                js.set("_djinni_js_proxy_id", id);
            } else {
                // already has id, look up in js proxy cache
                id = idProp.as<JsProxyId>();
                auto i = jsProxyCache.find(id);
                if (i != jsProxyCache.end()) {
                    auto strongProxyRef = i->second.lock();
                    if (strongProxyRef) {
                        std::cout << "existing js proxy" << std::endl;
                        return std::dynamic_pointer_cast<typename Self::JsProxy>(strongProxyRef);
                    }
                }
            }
            // not found or cache entry expired
            // create new js proxy and store it in cache
            auto newJsProxy = std::make_shared<typename Self::JsProxy>(js);
            jsProxyCache.emplace(id, newJsProxy);
            return newJsProxy;
        }
    }
};

struct DataObject {
    virtual ~DataObject() = default;
    em::val createJsObject();
    virtual unsigned addr() = 0;
    virtual unsigned size() = 0;
};

template<typename T>
class GenericBuffer: public DataObject {
public:
    GenericBuffer(size_t size) : _buffer(size, {}) {
        std::cout << "allocate direct buffer:@" << this
                  << " addr:" << reinterpret_cast<void*>(_buffer.data())
                  << " size:" << _buffer.size()
                  << std::endl;
    }
    GenericBuffer(const typename T::value_type* data, typename T::size_type size) :
        _buffer(data, data + size) {
        std::cout << "initialize direct buffer:@" << this
                  << " addr:" << reinterpret_cast<void*>(_buffer.data())
                  << " size:" << _buffer.size()
                  << std::endl;
    }
    GenericBuffer(T&& toTakeOver) : _buffer(std::move(toTakeOver)) {
        std::cout << "direct buffer takeover:@" << this
                  << " addr:" << reinterpret_cast<void*>(_buffer.data())
                  << " size:" << _buffer.size()
                  << std::endl;
    }
    ~GenericBuffer() override {
        std::cout << "free direct buffer:@" << this
                  << " size:" << _buffer.size()
                  << std::endl;
    }
    unsigned addr() override {
        return reinterpret_cast<unsigned>(_buffer.data());
    }
    unsigned size() override {
        return static_cast<unsigned>(_buffer.size());
    }
private:
    T _buffer;
};

}

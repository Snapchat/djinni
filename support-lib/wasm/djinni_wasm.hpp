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

#include <emscripten.h>
#include <emscripten/bind.h>

#ifdef __EMSCRIPTEN_PTHREADS__
#include <emscripten/threading.h>
#endif

// #include "../cpp/expected.hpp"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <optional>
#include <stdexcept>
#include <iostream>

namespace em = emscripten;

namespace djinni {

extern em::val getCppProxyFinalizerRegistry();
extern em::val getCppProxyClass();
extern em::val getWasmMemoryBuffer();

template<typename T>
class InstanceTracker {
#ifdef DJINNI_WASM_TRACK_INSTANCES
    static int& count() {
        static int theCount = 0;
        return theCount;
    }
public:
    InstanceTracker() {
        std::cout << "++" << typeid(this).name() << " => " << ++count() << std::endl;
    }
    virtual ~InstanceTracker() {
        std::cout << "--" << typeid(this).name() << " => " << --count() << std::endl;
    }
#endif
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

    static em::val getArrayClass();
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
        return c ? T::Boxed::fromCpp(*c) : em::val::undefined();
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

// // js success type:
// // { result: ...}
// // js error type:
// // { error: ...}
// template <class Result, class Error>
// struct Outcome
// {
//     using CppType = expected<typename Result::CppType, typename Error::CppType>;
//     using JsType = em::val;
//     using Boxed = Outcome;
    
//     static CppType toCpp(const JsType& j) {
//         em::val res = j["result"];
//         if (!res.isUndefined()) {
//             return Result::Boxed::toCpp(res);
//         } else {
//             em::val err = j["error"];
//             assert(!err.isUndefined());
//             return make_unexpected(Error::Boxed::toCpp(err));
//         }
//     }
//     static JsType fromCpp(const CppType& c) {
//         if (c.has_value()) {
//             em::val res = em::val::object();
//             res.set("result", Result::Boxed::fromCpp(c.value()));
//             return res;
//         } else {
//             em::val err = em::val::object();
//             err.set("error", Error::Boxed::fromCpp(c.error()));
//             return err;
//         }
//     }
// };

template<char... chars>
class JsClassName {
public:
    static em::val resolve() {
        static em::val cls = [] {
            std::string name = {chars...};
            em::val ns = em::val::module_property("protobuf");
            std::string::size_type i = 0;
            std::string::size_type j = name.find('.', i);
            while (j != std::string::npos) {
                ns = ns[name.substr(i, j - i)];
                i = j + 1;
                j = name.find('.', i);
            }
            return ns[name.substr(i)];
        }();
        return cls;
    }
};

template<typename CppProto, typename JsProto>
class Protobuf {
public:
    using CppType = CppProto;
    using JsType = em::val;

    using Boxed = Protobuf;

    static CppType toCpp(JsType j)
    {
        em::val jsClass = JsProto::resolve();
        auto writer = jsClass.call<em::val>("encode", j);
        auto bytes = writer.call<em::val>("finish");
        size_t length = bytes["byteLength"].as<int>();
        CppProto ret;
        if (bytes["buffer"] == getWasmMemoryBuffer()) {
            const void* pbytes = reinterpret_cast<void*>(bytes["byteOffset"].as<unsigned>());
            ret.ParseFromArray(pbytes, static_cast<int>(length));
        } else {
            std::vector<uint8_t> cbuf(length);
            static em::val writeNativeMemory = em::val::module_property("writeNativeMemory");
            writeNativeMemory(bytes, reinterpret_cast<uint32_t>(cbuf.data()));
            ret.ParseFromArray(cbuf.data(), static_cast<int>(length));
        }
        return ret;
    }
        
    static JsType fromCpp(const CppType& c)
    {
        std::vector<uint8_t> cbuf(c.ByteSizeLong());
        c.SerializeToArray(cbuf.data(), static_cast<int>(cbuf.size()));

        unsigned addr = reinterpret_cast<unsigned>(cbuf.data());
        unsigned size = static_cast<unsigned>(cbuf.size());
        static auto uint8ArrayClass = em::val::global("Uint8Array");
        em::val array = uint8ArrayClass.new_(::djinni::getWasmMemoryBuffer(), addr, size);
        em::val jsClass = JsProto::resolve();
        return jsClass.call<em::val>("decode", array);
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

template <typename T, typename U = Array<T>>
struct PrimitiveArray {
    using CppType = std::vector<typename T::CppType>;
    using JsType = em::val;
    using Boxed = PrimitiveArray;

    static CppType toCpp(const JsType& j) {
        const size_t length = j["length"].as<size_t>();
        std::vector<typename T::CppType> c(length);
        static em::val writeNativeMemory = em::val::module_property("writeNativeMemory");
        writeNativeMemory(j, reinterpret_cast<uint32_t>(c.data()));
        return c;
    }
    static JsType fromCpp(const CppType& c) {
        static em::val readNativeMemory = em::val::module_property("readNativeMemory");
        return readNativeMemory(U::getArrayClass(), reinterpret_cast<uint32_t>(c.data()),
                                static_cast<uint32_t>(c.size() * sizeof(typename T::CppType)));
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
struct Array<I64> : PrimitiveArray<I64> {
    static em::val getArrayClass() {
        static em::val arrayClass = em::val::global("BigInt64Array");
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

class CppResolveHandlerBase {
public:
    virtual ~CppResolveHandlerBase() = default;
    virtual void init(em::val resolveFunc, em::val rejectFunc) = 0;

    static void initInstance(int handlerPtr, em::val resolveFunc, em::val rejectFunc) {
        auto* handler = reinterpret_cast<CppResolveHandlerBase*>(handlerPtr);
        handler->init(resolveFunc, rejectFunc);
    }
    
    static void resolveNativePromise(int func, int context, em::val res, em::val err) {
        typedef void (*ResolveNativePromiseFunc)(int context, em::val res, em::val err);
        auto resolveNativePromiseFunc = reinterpret_cast<ResolveNativePromiseFunc>(func);
        resolveNativePromiseFunc(context, res, err);
    }
};

using JsProxyId = uint64_t;

class JsException : public std::runtime_error {
public:
    JsException(const std::string& what): std::runtime_error(what) {}
};

class JsProxyBase {
public:
    JsProxyBase(const em::val& v);
    virtual ~JsProxyBase();
    const em::val& _jsRef() const;
    template<typename ...Args>
    em::val callMethod(const std::string& name, Args&&... args) {
        em::val caller = em::val::module_property("callJsProxyMethod");
        return caller(_js, name, std::forward<Args>(args)...);
    }
protected:
    void checkError(const em::val& v);
private:
    em::val _js;
    JsProxyId _id;
};

extern JsProxyId nextId;
extern std::unordered_map<JsProxyId, std::weak_ptr<JsProxyBase>> jsProxyCache;
extern std::unordered_map<void*, em::val> cppProxyCache;
extern std::mutex jsProxyCacheMutex;
extern std::mutex cppProxyCacheMutex;

template<typename I, typename Self>
struct JsInterface {
    static void nativeDestroy(const std::shared_ptr<I>& cpp) {
        std::lock_guard lk(cppProxyCacheMutex);
        assert(cppProxyCache.find(cpp.get()) != cppProxyCache.end());
        cppProxyCache.erase(cpp.get());
    }
    // enable this only when the derived class has `cppProxyMethods` defined
    // (interface +c)
    template <typename, typename>
    struct GetOrCreateCppProxy {
        em::val operator() (const std::shared_ptr<I>& c) {
            assert(false && "Attempting to pass C++ object but interface lacks +c");
            return em::val::undefined();
        }
    };
    template <typename T>
    struct GetOrCreateCppProxy<T, std::void_t<decltype(T::cppProxyMethods)>> {
        em::val operator() (const std::shared_ptr<I>& c) {
            // look up in cpp proxy cache
            std::lock_guard lk(cppProxyCacheMutex);
            auto i = cppProxyCache.find(c.get());
            if (i != cppProxyCache.end()) {
                // found existing cpp proxy
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
    };
    static em::val _toJs(const std::shared_ptr<I>& c) {
        if (c == nullptr) {
            // null object
            return em::val::null();
        }
        else if (auto* p = dynamic_cast<JsProxyBase*>(c.get())) {
            // unwrap existing js proxy
            return p->_jsRef();
        } else {
            return GetOrCreateCppProxy<Self, void>()(c);
        }
    }
    // enable this only when the derived class has `JsProxy` defined
    // (interface +w)
    template <typename, typename>
    struct GetOrCreateJsProxy {
        std::shared_ptr<I> operator() (em::val js) {
            assert(false && "Attempting to pass JS object but interface lacks +w");
            return {};
        }
    };
    template <typename T>
    struct GetOrCreateJsProxy<T, std::void_t<typename T::JsProxy>> {
        std::shared_ptr<I> operator() (em::val js) {
            std::lock_guard lk(jsProxyCacheMutex);
            // check prsence of proxy id in js object
            JsProxyId id;
            auto idProp = js["_djinni_js_proxy_id"];
            if (idProp.isUndefined()) {
                // no id, assign a new id
                id = nextId++;
                js.set("_djinni_js_proxy_id", id);
            } else {
                // already has id, look up in js proxy cache
                id = idProp.as<JsProxyId>();
                auto i = jsProxyCache.find(id);
                if (i != jsProxyCache.end()) {
                    auto strongProxyRef = i->second.lock();
                    if (strongProxyRef) {
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
    };

    static std::shared_ptr<I> _fromJs(em::val js) {
        static const em::val nativeRef("_djinni_native_ref");
        // null object
        if (js.isUndefined() || js.isNull()) {
            return {};
        }
        else if (nativeRef.in(js)) {
            // existing cpp proxy
            return js[nativeRef].as<std::shared_ptr<I>>();
        } else {
            return GetOrCreateJsProxy<Self, void>()(js);
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
    GenericBuffer(size_t size) : _buffer(size, {}) {}
    GenericBuffer(const typename T::value_type* data, typename T::size_type size) :
        _buffer(data, data + size) {}
    GenericBuffer(T&& toTakeOver) : _buffer(std::move(toTakeOver)) {}
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

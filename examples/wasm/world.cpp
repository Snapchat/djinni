#include <emscripten.h>
#include <emscripten/bind.h>

#include "expected.hpp"

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <optional>
#include <algorithm>

using namespace emscripten;
using djinni::expected;
using djinni::make_unexpected;

template<typename T>
class InstanceTracker {
    static int& count() {
        static int theCount = 0;
        return theCount;
    }
public:
    InstanceTracker() {
        std::cout << "++" << typeid(T).name() << " => " << ++count() << std::endl;
    }
    virtual ~InstanceTracker() {
        std::cout << "--" << typeid(T).name() << " => " << --count() << std::endl;
    }
};

// library code
template<typename T>
class Primitive {
public:
    using CppType = T;
    using JsType = T;

    struct Boxed {
        using JsType = val;
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

class Binary {
public:
    using CppType = std::vector<uint8_t>;
    using JsType = val;
    using Boxed = Binary;
    
    static CppType toCpp(const JsType& j) {
        return convertJSArrayToNumberVector<uint8_t>(j);
    }
    static JsType fromCpp(const CppType& c) {
        val memoryView{ typed_memory_view(c.size(), c.data()) };
        return memoryView.call<val>("slice", 0);
    }
};

class Date {
public:
    using CppType = std::chrono::system_clock::time_point;
    using JsType = val;
    using Boxed = Date;
    
    static CppType toCpp(const JsType& j) {
        auto milliesSinceEpoch = std::chrono::milliseconds(static_cast<int64_t>(j.call<val>("getTime").as<double>()));
        return CppType(milliesSinceEpoch);
    }
    static JsType fromCpp(const CppType& c) {
        auto milliesSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(c.time_since_epoch());
        static val dateType = val::global("Date");
        return dateType.new_(static_cast<double>(milliesSinceEpoch.count()));
    }
};

template <template <class> class OptionalType, class T>
struct Optional
{
    template <typename C> static OptionalType<typename C::CppType> opt_type(...);
    template <typename C> static typename C::CppOptType opt_type(typename C::CppOptType *);
    using CppType = decltype(opt_type<T>(nullptr));
    using JsType = val;
    using Boxed = Optional;
    
    static CppType toCpp(const JsType& j) {
        if (j.isUndefined() || j.isNull()) {
            return CppType{};
        } else {
            return T::Boxed::toCpp(j);
        }
    }
    static JsType fromCpp(const OptionalType<typename T::CppType>& c) {
        return c ? T::Boxed::fromCpp(*c) : val::null();
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
    using JsType = val;
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
        val new_array = val::array();
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
    using JsType = val;
    using Boxed = Set;

    static CppType toCpp(const JsType& j) {
        static val arrayClass = val::global("Array");
        val entries = arrayClass.call<val>("from", j);
        const size_t l = entries["length"].as<size_t>();
        CppType rs;
        for (size_t i = 0; i < l; ++i) {
            rs.insert(T::Boxed::toCpp(entries[i]));
        }
        return rs;
    }
    static JsType fromCpp(const CppType& c) {
        static val setClass = val::global("Set");
        val newSet = setClass.new_();
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
    using JsType = val;
    using Boxed = Map;

    static CppType toCpp(const JsType& j) {
        static val arrayClass = val::global("Array");
        val entries = arrayClass.call<val>("from", j);
        const size_t l = entries["length"].as<size_t>();
        CppType rm;
        for (size_t i = 0; i < l; ++i) {
            rm.emplace(Key::Boxed::toCpp(entries[i][0]),
                       Value::Boxed::toCpp(entries[i][1]));
        }
        return rm;
    }
    static JsType fromCpp(const CppType& c) {
        static val mapClass = val::global("Map");
        val newMap = mapClass.new_();
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
    using JsType = val;
    using Boxed = Outcome;
    
    static CppType toCpp(const JsType& j) {
        val res = j["result"];
        if (!res.isUndefined()) {
            return RESULT::Boxed::toCpp(res);
        } else {
            val err = j["error"];
            assert(!err.isUndefined());
            return make_unexpected(ERROR::Boxed::toCpp(err));
        }
    }
    static JsType fromCpp(const CppType& c) {
        if (c.has_value()) {
            val res = val::object();
            res.set("result", RESULT::Boxed::fromCpp(c.value()));
            return res;
        } else {
            val err = val::object();
            err.set("error", ERROR::Boxed::fromCpp(c.error()));
            return err;
        }
    }
};

template <typename T>
struct TypedArrayTraits {
    static val getArrayClass() {
        return val::undefined();
    }
};
template<>
struct TypedArrayTraits<I8> {
    static val getArrayClass() {
        static val arrayClass = val::global("Int8Array");
        return arrayClass;
    }
};
template<>
struct TypedArrayTraits<I16> {
    static val getArrayClass() {
        static val arrayClass = val::global("Int16Array");
        return arrayClass;
    }
};
template<>
struct TypedArrayTraits<I32> {
    static val getArrayClass() {
        static val arrayClass = val::global("Int32Array");
        return arrayClass;
    }
};
template<>
struct TypedArrayTraits<F32> {
    static val getArrayClass() {
        static val arrayClass = val::global("Float32Array");
        return arrayClass;
    }
};
template<>
struct TypedArrayTraits<F64> {
    static val getArrayClass() {
        static val arrayClass = val::global("Float64Array");
        return arrayClass;
    }
};

template <typename T>
struct Array {
    using CppType = std::vector<typename T::CppType>;
    using JsType = val;
    using Boxed = Array;

    static CppType toCpp(const JsType& j) {
        static val arrayBufferClass = val::global("ArrayBuffer");
        if (j["buffer"].instanceof(arrayBufferClass)) {
            std::cout << "typed array to vector" << std::endl;
            return convertJSArrayToNumberVector<typename T::CppType>(j);
        } else {
            return List<T>::toCpp(j);
        }
    }
    static JsType fromCpp(const CppType& c) {
        val arrayClass = TypedArrayTraits<T>::getArrayClass();
        if (!arrayClass.isUndefined()) {
            std::cout << "vector to typed array" << std::endl;
            val memoryView{ typed_memory_view(c.size(), c.data()) };
            val buffer = memoryView.call<val>("slice", 0);
            return arrayClass.new_(buffer);
        } else {
            return List<T>::fromCpp(c);
        }
    }
};

using JsProxyId = int32_t;
static JsProxyId nextId = 0;
std::unordered_map<JsProxyId, void*> jsProxyCache;
std::unordered_map<void*, val> cppProxyCache;

class JsProxyBase {
public:
    JsProxyBase(const val& v) : _js(v), _id(_js["_djinni_js_proxy_id"].as<JsProxyId>()) {
        jsProxyCache.emplace(_id, this);
    }
    virtual ~JsProxyBase() {
        jsProxyCache.erase(_id);
    }
    const val& _jsRef() const {
        return _js;
    }
private:
    val _js;
    JsProxyId _id;
};

template<typename I, typename Self>
struct JsInterface {
    static void nativeDestroy(const std::shared_ptr<I>& cpp) {
        std::cout << "delete entry from cppProxyCache" << std::endl;
        assert(cppProxyCache.find(cpp.get()) != cppProxyCache.end());
        cppProxyCache.erase(cpp.get());
    }
    static val _toJs(const std::shared_ptr<I>& c) {
        if (c == nullptr) {
            return val::null();
        }
        else if (auto* p = dynamic_cast<JsProxyBase*>(c.get())) {
            // unwrap js object
            std::cout << "unwrap js object" << std::endl;
            return p->_jsRef();
        } else {
            auto i = cppProxyCache.find(c.get());
            if (i != cppProxyCache.end()) {
                // existing cpp proxy
                std::cout << "already has cpp proxy" << std::endl;
                return i->second.template call<val>("deref");
            } else {
                static val weakRefClass = val::global("WeakRef");
                val nativeRef(c);
                val cppProxy = Self::cppProxy().new_(nativeRef);
                val weakRef = weakRefClass.new_(cppProxy);
                cppProxyCache.emplace(c.get(), weakRef);
                static val finalizerRegistry = val::module_property("cppProxyFinalizerRegistry");
                finalizerRegistry.call<void>("register", cppProxy, nativeRef);
                return cppProxy;
            }
        }
    }
    static std::shared_ptr<I> _fromJs(val js) {
        static const val nativeRef("_djinni_native_ref");
        if (js.isUndefined() || js.isNull()) {
            return {};
        }
        else if (nativeRef.in(js)) { // is cpp object
            std::cout << "getting cpp object" << std::endl;
            return js[nativeRef].as<std::shared_ptr<I>>();
        } else { // is jsproxy
            JsProxyId id;
            auto idProp = js["_djinni_js_proxy_id"];
            if (idProp.isUndefined()) {
                id = nextId++;
                std::cout << "assign proxy id " << id << std::endl;
                js.set("_djinni_js_proxy_id", id);
            } else {
                id = idProp.as<JsProxyId>();
            }
            auto i = jsProxyCache.find(id);
            if (i != jsProxyCache.end()) {
                std::cout << "existing js proxy" << std::endl;
                return reinterpret_cast<typename Self::JsProxy*>(i->second)->shared_from_this();
            } else {
                return std::make_shared<typename Self::JsProxy>(js);
            }
        }
    }
};

// djinni generated interface
struct MyRecord {
    int32_t _x;
    std::string _y;

    MyRecord(int32_t x, const std::string& y) : _x(x), _y(y) {}
};

class MyInterface {
public:
    virtual ~MyInterface() = default;
    virtual void foo(int32_t x) = 0;
    virtual std::wstring testStr(const std::wstring& x) = 0;
    virtual std::vector<uint8_t> testBin(const std::vector<uint8_t>& bin) = 0;
    virtual std::chrono::system_clock::time_point testDate(const std::chrono::system_clock::time_point& d) = 0;
    virtual MyRecord testRecord(const MyRecord& r) = 0;
    virtual std::optional<MyRecord> testOptional1(const std::optional<MyRecord>& o) = 0;
    virtual std::shared_ptr<MyInterface> testOptional2(const std::shared_ptr<MyInterface>& o) = 0;
    virtual std::vector<MyRecord> testList(const std::vector<MyRecord>& l) = 0;
    virtual std::unordered_set<std::string> testSet(const std::unordered_set<std::string>& s) = 0;
    virtual std::unordered_map<std::string, MyRecord> testMap(const std::unordered_map<std::string, MyRecord>& m) = 0;
    virtual expected<MyRecord, std::string> testOutcome(bool success) = 0;

    static std::shared_ptr<MyInterface> create();
    static std::shared_ptr<MyInterface> instance();
    static std::shared_ptr<MyInterface> pass(const std::shared_ptr<MyInterface>& i);
    static bool comp(const std::shared_ptr<MyInterface>& i, const std::shared_ptr<MyInterface>& j);
    static void testOutcome2(const std::shared_ptr<MyInterface>& i);
    static std::vector<int32_t> testArray(const std::vector<int32_t>& a);
};

// djinni generated stubs
struct NativeMyRecord {
    using CppType = MyRecord;
    using JsType = val;
    using Boxed = NativeMyRecord;
    
    static CppType toCpp(const JsType& j)
    {
        return CppType (I32::Boxed::toCpp(j["_x"]),
                        String::Boxed::toCpp(j["_y"]));
    }
    static JsType fromCpp(const CppType& c)
    {
        val js = val::object();
        js.set("_x", I32::Boxed::fromCpp(c._x));
        js.set("_y", String::Boxed::fromCpp(c._y));
        return js;
    }
};

struct NativeMyInterface : JsInterface<MyInterface, NativeMyInterface> {
    class JsProxy: public JsProxyBase,
                   public MyInterface,
                   public InstanceTracker<JsProxy>,
                   public std::enable_shared_from_this<JsProxy> {
    public:
        JsProxy(const val& v) : JsProxyBase(v) {}
        void foo(int x) override {
            return _jsRef().call<void>("foo", x);
        }
        std::wstring testStr(const std::wstring& x) override {
            return WString::toCpp(_jsRef().call<std::wstring>("testStr", WString::fromCpp(x)));
        }
        std::vector<uint8_t> testBin(const std::vector<uint8_t>& bin) override {
            return Binary::toCpp(_jsRef().call<val>("testBin", Binary::fromCpp(bin)));
        }
        std::chrono::system_clock::time_point testDate(const std::chrono::system_clock::time_point& d) override {
            return Date::toCpp(_jsRef().call<val>("testDate", Date::fromCpp(d)));
        }
        MyRecord testRecord(const MyRecord& r) override {
            return NativeMyRecord::toCpp(_jsRef().call<val>("testRecord", NativeMyRecord::fromCpp(r)));
        }
        std::optional<MyRecord> testOptional1(const std::optional<MyRecord>& o) override {
            return Optional<std::optional, NativeMyRecord>::toCpp(
                _jsRef().call<val>("testOptional1",
                                   Optional<std::optional, NativeMyRecord>::fromCpp(o)));
        }
        std::shared_ptr<MyInterface> testOptional2(const std::shared_ptr<MyInterface>& o) override {
            return Optional<std::optional, NativeMyInterface>::toCpp(
                _jsRef().call<val>("testOptional2",
                                   Optional<std::optional, NativeMyInterface>::fromCpp(o)));
        }
        std::vector<MyRecord> testList(const std::vector<MyRecord>& l) override {
            return List<NativeMyRecord>::toCpp(_jsRef().call<val>("testList", List<NativeMyRecord>::fromCpp(l)));
        }
        std::unordered_set<std::string> testSet(const std::unordered_set<std::string>& s) override {
            return Set<String>::toCpp(_jsRef().call<val>("testSet", Set<String>::fromCpp(s)));
        }
        std::unordered_map<std::string, MyRecord> testMap(const std::unordered_map<std::string, MyRecord>& m) override {
            return Map<String, NativeMyRecord>::toCpp(_jsRef().call<val>("testMap", Map<String, NativeMyRecord>::fromCpp(m)));
        }
        expected<MyRecord, std::string> testOutcome(bool success) override {
            return Outcome<NativeMyRecord, String>::toCpp(_jsRef().call<val>("testOutcome", Bool::fromCpp(success)));
        }
    };
    static val cppProxy() {
        static val inst = val::module_property("MyInterface_CppProxy");
        return inst;
    }
    // ---------
    using CppType = std::shared_ptr<MyInterface>;
    using CppOptType = std::shared_ptr<MyInterface>;
    using JsType = val;
    using Boxed = NativeMyInterface;
    static CppType toCpp(JsType j) { return _fromJs(j); }
    static JsType fromCppOpt(const CppOptType& c) { return {_toJs(c)}; }
    static JsType fromCpp(const CppType& c) { return fromCppOpt(c); }
    // ---------
    static void foo(const std::shared_ptr<MyInterface>& self, int32_t x) {
        return self->foo(I32::toCpp(x));
    }
    static std::wstring testStr(const std::shared_ptr<MyInterface>& self, const std::wstring& x) {
        return WString::fromCpp(self->testStr(WString::toCpp(x)));
    }
    static val testBin(const std::shared_ptr<MyInterface>& self, const val& bin) {
        return Binary::fromCpp(self->testBin(Binary::toCpp(bin)));
    }
    static val testDate(const std::shared_ptr<MyInterface>& self, const val& d) {
        return Date::fromCpp(self->testDate(Date::toCpp(d)));
    }
    static val testRecord(const std::shared_ptr<MyInterface>& self, const val& r) {
        return NativeMyRecord::fromCpp(self->testRecord(NativeMyRecord::toCpp(r)));
    }
    static val testOptional1(const std::shared_ptr<MyInterface>& self, const val& o) {
        return Optional<std::optional, NativeMyRecord>::fromCpp(self->testOptional1(Optional<std::optional, NativeMyRecord>::toCpp(o)));
    }
    static val testOptional2(const std::shared_ptr<MyInterface>& self, const val& o) {
        return Optional<std::optional, NativeMyInterface>::fromCpp(self->testOptional2(Optional<std::optional, NativeMyInterface>::toCpp(o)));
    }
    static val testList(const std::shared_ptr<MyInterface>& self, const val& l) {
        return List<NativeMyRecord>::fromCpp(self->testList(List<NativeMyRecord>::toCpp(l)));
    }
    static val testSet(const std::shared_ptr<MyInterface>& self, const val& s) {
        return Set<String>::fromCpp(self->testSet(Set<String>::toCpp(s)));
    }
    static val testMap(const std::shared_ptr<MyInterface>& self, const val& m) {
        return Map<String, NativeMyRecord>::fromCpp(self->testMap(Map<String, NativeMyRecord>::toCpp(m)));
    }
    static val testOutcome(const std::shared_ptr<MyInterface>& self, bool success) {
        return Outcome<NativeMyRecord, String>::fromCpp(self->testOutcome(Bool::toCpp(success)));
    }
    static val create() {
        return NativeMyInterface::fromCpp(MyInterface::create());
    }
    static val instance() {
        return NativeMyInterface::fromCpp(MyInterface::instance());
    }
    static val pass(const val& i) {
        return NativeMyInterface::fromCpp(MyInterface::pass(NativeMyInterface::toCpp(i)));
    }
    static bool comp(const val& i, const val& j) {
        return Bool::fromCpp(MyInterface::comp(NativeMyInterface::toCpp(i), NativeMyInterface::toCpp(j)));
    }
    static void testOutcome2(const val& i) {
        return MyInterface::testOutcome2(NativeMyInterface::toCpp(i));
    }
    static val testArray(const val& a) {
        return Array<I32>::fromCpp(MyInterface::testArray(Array<I32>::toCpp(a)));
    }
};

// djinni generated binding
EMSCRIPTEN_BINDINGS(MyInterface) {
    class_<MyInterface>("MyInterface")
        .smart_ptr<std::shared_ptr<MyInterface>>("MyInterface")
        .function("foo", &NativeMyInterface::foo)
        .function("testStr", &NativeMyInterface::testStr)
        .function("testBin", &NativeMyInterface::testBin)
        .function("testDate", &NativeMyInterface::testDate)
        .function("testRecord", &NativeMyInterface::testRecord)
        .function("testOptional1", &NativeMyInterface::testOptional1)
        .function("testOptional2", &NativeMyInterface::testOptional2)
        .function("testList", &NativeMyInterface::testList)
        .function("testSet", &NativeMyInterface::testSet)
        .function("testMap", &NativeMyInterface::testMap)
        .function("testOutcome", &NativeMyInterface::testOutcome)
        .class_function("create", &NativeMyInterface::create)
        .class_function("instance", &NativeMyInterface::instance)
        .class_function("pass", &NativeMyInterface::pass)
        .class_function("comp", &NativeMyInterface::comp)
        .class_function("testOutcome2", &NativeMyInterface::testOutcome2)
        .class_function("testArray", &NativeMyInterface::testArray)
        .function("nativeDestroy", &NativeMyInterface::nativeDestroy)
        ;
}

// user implementation
class MyInterfaceImpl : public MyInterface, public InstanceTracker<MyInterfaceImpl> {
public:
    MyInterfaceImpl() {}

    void foo(int32_t x) override {
        std::cout << "MyInterfaceImpl::foo(" << x <<")" << std::endl;
    }
    std::wstring testStr(const std::wstring& x) override {
        return x + L" 123";
    }
    std::vector<uint8_t> testBin(const std::vector<uint8_t>& bin) override {
        auto v = bin;
        v.push_back(123);
        return v;
    }
    std::chrono::system_clock::time_point testDate(const std::chrono::system_clock::time_point& d) override {
        return d + std::chrono::hours(240);
    }
    MyRecord testRecord(const MyRecord& r) override {
        return MyRecord(r._x + 100, r._y);
    }
    std::optional<MyRecord> testOptional1(const std::optional<MyRecord>& o) override {
        if (o) {
            return MyRecord(o->_x + 100, o->_y);
        } else {
            return {};
        }
    }
    std::shared_ptr<MyInterface> testOptional2(const std::shared_ptr<MyInterface>& o) override {
        if (o) {
            o->foo(111);
            return MyInterface::create();
        } else {
            return {};
        }
    }
    std::vector<MyRecord> testList(const std::vector<MyRecord>& l) override {
        auto copy = l;
        std::reverse(copy.begin(), copy.end());
        return copy;
    }
    std::unordered_set<std::string> testSet(const std::unordered_set<std::string>& s) override {
        std::unordered_set<std::string> r;
        for (const auto& e: s) {
            r.emplace(e + " xxxx");
        }
        return r;
    }
    std::unordered_map<std::string, MyRecord> testMap(const std::unordered_map<std::string, MyRecord>& m) override {
        std::unordered_map<std::string, MyRecord> r;
        for (const auto &[k, v] : m) {
            r.emplace(v._y, MyRecord{v._x, k});
        }
        return r;
    }
    expected<MyRecord, std::string> testOutcome(bool success) override {
        if (success) {
            return MyRecord(100, "hello");
        } else {
            return make_unexpected("BAD");
        }
    }
};

std::shared_ptr<MyInterface> MyInterface::create() {
    return std::make_shared<MyInterfaceImpl>();
}

std::shared_ptr<MyInterface> MyInterface::instance() {
    static auto singleton = std::make_shared<MyInterfaceImpl>();
    return singleton;
}

std::shared_ptr<MyInterface> MyInterface::pass(const std::shared_ptr<MyInterface>& i) {
    return i;
}

void MyInterface::testOutcome2(const std::shared_ptr<MyInterface>& i) {
    auto outcome1 = i->testOutcome(true);
    if (outcome1.has_value()) {
        std::cout << "success" << std::endl;
        std::cout << outcome1.value()._x << std::endl;
        std::cout << outcome1.value()._y << std::endl;
    }
    auto outcome2 = i->testOutcome(false);
    if (!outcome2.has_value()) {
        std::cout << "error" << std::endl;
        std::cout << outcome2.error() << std::endl;
    }
}

std::vector<int32_t> MyInterface::testArray(const std::vector<int32_t>& a) {
    std::vector<int32_t> copy = a;
    std::reverse(copy.begin(), copy.end());
    return copy;
}

bool MyInterface::comp(const std::shared_ptr<MyInterface>& i, const std::shared_ptr<MyInterface>& j) {
    return i == j;
}

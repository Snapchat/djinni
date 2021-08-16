#include <emscripten.h>
#include <emscripten/bind.h>

#include <iostream>
#include <map>
#include <vector>
#include <optional>

using namespace emscripten;

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

    struct Boxed
    {
        using JsType = val;
        static CppType toCpp(JsType j)
        {
            return j.as<CppType>();
        }
        static JsType fromCpp(CppType c)
        {
            return JsType(c);
        }
    };

    static CppType toCpp(const JsType& j)
    {
        return j;
    }
    static JsType fromCpp(const CppType& c)
    {
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
    
    static CppType toCpp(const JsType& j)
    {
        return convertJSArrayToNumberVector<uint8_t>(j);
    }
    static JsType fromCpp(const CppType& c)
    {
        val memoryView{ typed_memory_view(c.size(), c.data()) };
        return memoryView.call<val>("slice", 0);
    }
};

class Date {
public:
    using CppType = std::chrono::system_clock::time_point;
    using JsType = val;
    using Boxed = Date;
    
    static CppType toCpp(const JsType& j)
    {
        auto milliesSinceEpoch = std::chrono::milliseconds(static_cast<int64_t>(j.call<val>("getTime").as<double>()));
        return CppType(milliesSinceEpoch);
    }
    static JsType fromCpp(const CppType& c)
    {
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
    
    static CppType toCpp(const JsType& j)
    {
        if (j.isUndefined() || j.isNull()) {
            return CppType{};
        } else {
            return T::Boxed::toCpp(j);
        }
    }
    static JsType fromCpp(const OptionalType<typename T::CppType>& c)
    {
        return c ? T::Boxed::fromCpp(*c) : val::null();
    }
    template <typename C = T>
    static JsType fromCpp(const typename C::CppOptType& cppOpt) {
        return T::Boxed::fromCppOpt(cppOpt);
    }
};

using JsProxyId = int32_t;
std::map<JsProxyId, void*> jsProxyCache;
std::map<void*, val> cppProxyCache;

class JsProxyBase {
public:
    JsProxyBase(const val& v) : _js(v), _id(_js["_jsProxyId"].as<JsProxyId>()) {
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
        static const val nativeRef("_nativeRef");
        if (js.isUndefined() || js.isNull()) {
            return {};
        }
        else if (nativeRef.in(js)) { // is cpp object
            std::cout << "getting cpp object" << std::endl;
            return js[nativeRef].as<std::shared_ptr<I>>();
        } else { // is jsproxy
            static JsProxyId nextId = 0;
            JsProxyId id;
            auto idProp = js["_jsProxyId"];
            if (idProp.isUndefined()) {
                id = nextId++;
                std::cout << "assign proxy id " << id << std::endl;
                js.set("_jsProxyId", id);
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

    static std::shared_ptr<MyInterface> create();
    static std::shared_ptr<MyInterface> instance();
    static std::shared_ptr<MyInterface> pass(const std::shared_ptr<MyInterface>& i);
    static bool comp(const std::shared_ptr<MyInterface>& i, const std::shared_ptr<MyInterface>& j);
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
        .class_function("create", &NativeMyInterface::create)
        .class_function("instance", &NativeMyInterface::instance)
        .class_function("pass", &NativeMyInterface::pass)
        .class_function("comp", &NativeMyInterface::comp)
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

bool MyInterface::comp(const std::shared_ptr<MyInterface>& i, const std::shared_ptr<MyInterface>& j) {
    return i == j;
}

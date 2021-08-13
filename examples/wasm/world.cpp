#include <emscripten.h>
#include <emscripten/bind.h>

#include <iostream>
#include <map>
#include <vector>

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
    using Boxed = Primitive;

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
        static val bigintType = val::global("BigInt");
        auto milliesSinceEpoch = std::chrono::milliseconds(bigintType(j.call<val>("getTime")).as<int64_t>());
        return CppType(milliesSinceEpoch);
    }
    static JsType fromCpp(const CppType& c)
    {
        auto milliesSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(c.time_since_epoch());
        static val dateType = val::global("Date");
        static val number = val::global("Number");
        auto d = dateType.new_();
        d.call<void>("setTime", number(milliesSinceEpoch.count()));
        return d;
    }
};

// TODO: optimize
struct JsProxyCacheEntry {
    val js;
    void* cpp;
};
std::vector<JsProxyCacheEntry> jsProxyCache;
std::map<void*, val> cppProxyCache;

class JsProxyBase {
public:
    JsProxyBase(const val& v) : _js(v) {
        jsProxyCache.emplace_back(JsProxyCacheEntry{_js, this});
    }

    virtual ~JsProxyBase() {
        jsProxyCache.erase(std::remove_if(jsProxyCache.begin(), jsProxyCache.end(), [&](const auto& x){
            return x.js == _js;
        }), jsProxyCache.end());
    }

    const val& _jsRef() const {
        return _js;
    }
private:
    val _js;
};

template<typename I, typename Self>
struct JsInterface {
    static void nativeDestroy(const std::shared_ptr<I>& cpp) {
        std::cout << "delete entry from cppProxyCache" << std::endl;
        assert(cppProxyCache.find(cpp.get()) != cppProxyCache.end());
        cppProxyCache.erase(cpp.get());
    }
    static val _toJs(const std::shared_ptr<I>& c) {
        if (auto* p = dynamic_cast<JsProxyBase*>(c.get())) {
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
                static val finalizerRegistry = val::module_property("djinniFinalizerRegistry");
                finalizerRegistry.call<void>("register", cppProxy, nativeRef);
                return cppProxy;
            }
        }
    }
    static std::shared_ptr<I> _fromJs(const val& js) {
        static const val nativeRef("_nativeRef");
        if (nativeRef.in(js)) { // is cpp object
            std::cout << "getting cpp object" << std::endl;
            return js[nativeRef].as<std::shared_ptr<I>>();
        } else { // is jsproxy
            auto i = std::find_if(jsProxyCache.begin(), jsProxyCache.end(), [&](const auto& x){
                return x.js == js;
            });
            if (i != jsProxyCache.end()) {
                std::cout << "existing js proxy" << std::endl;
                return reinterpret_cast<typename Self::JsProxy*>(i->cpp)->shared_from_this();
            } else {
                return std::make_shared<typename Self::JsProxy>(js);
            }
        }
    }
};

// djinni generated interface
class MyInterface {
public:
    virtual ~MyInterface() = default;
    virtual void foo(int32_t x) = 0;
    virtual std::wstring testStr(const std::wstring& x) = 0;
    virtual std::vector<uint8_t> testBin(const std::vector<uint8_t>& bin) = 0;
    virtual std::chrono::system_clock::time_point testDate(const std::chrono::system_clock::time_point& d) = 0;

    static std::shared_ptr<MyInterface> create();
    static std::shared_ptr<MyInterface> instance();
    static std::shared_ptr<MyInterface> pass(const std::shared_ptr<MyInterface>& i);
    static bool comp(const std::shared_ptr<MyInterface>& i, const std::shared_ptr<MyInterface>& j);
};

// djinni generated stubs
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
    };
    static val cppProxy() {
        static val inst = val::module_property("MyInterface_CppProxy");
        return inst;
    }
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
    static val create() {
        return _toJs(MyInterface::create());
    }
    static val instance() {
        return _toJs(MyInterface::instance());
    }
    static val pass(const val& i) {
        return _toJs(MyInterface::pass(_fromJs(i)));
    }
    static bool comp(const val& i, const val& j) {
        return MyInterface::comp(_fromJs(i), _fromJs(j));
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

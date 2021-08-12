#include <emscripten.h>
#include <emscripten/bind.h>

#include <iostream>
#include <map>

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

class String {
public:
    using CppType = std::string;
    using JsType = val;
    using Boxed = String;
    
    static CppType toCpp(const JsType& j)
    {
        return j.as<CppType>();
    }
    static JsType fromCpp(const CppType& c)
    {
        return JsType{c};
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
    virtual void foo(int x) = 0;
    virtual std::string testStr(const std::string& x) = 0;
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
        std::string testStr(const std::string& x) override {
            return String::toCpp(_jsRef().call<val>("testStr", String::fromCpp(x)));
        }
    };
    static val cppProxy() {
        static val inst = val::module_property("MyInterface_CppProxy");
        return inst;
    }
    // ---------
    static void foo(const std::shared_ptr<MyInterface>& self, int x) {
        return self->foo(x);
    }
    static val testStr(const std::shared_ptr<MyInterface>& self, const val& x) {
        return String::fromCpp(self->testStr(String::toCpp(x)));
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

    void foo(int x) override {
        std::cout << "MyInterfaceImpl::foo(" << x <<")" << std::endl;
    }
    std::string testStr(const std::string& x) override {
        return x + " 123";
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

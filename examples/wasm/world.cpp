#include "../../support-lib/wasm/djinni_wasm.hpp"
#include <algorithm>

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
    virtual djinni::expected<MyRecord, std::string> testOutcome(bool success) = 0;

    static std::shared_ptr<MyInterface> create();
    static std::shared_ptr<MyInterface> instance();
    static std::shared_ptr<MyInterface> pass(const std::shared_ptr<MyInterface>& i);
    static bool comp(const std::shared_ptr<MyInterface>& i, const std::shared_ptr<MyInterface>& j);
    static void testOutcome2(const std::shared_ptr<MyInterface>& i);
    static std::vector<int32_t> testArray(const std::vector<int32_t>& a);
};

// djinni generated stubs
namespace djinni_generated {

struct NativeMyRecord {
    using CppType = MyRecord;
    using JsType = em::val;
    using Boxed = NativeMyRecord;
    
    static CppType toCpp(const JsType& j)
    {
        return CppType (djinni::I32::Boxed::toCpp(j["_x"]),
                        djinni::String::Boxed::toCpp(j["_y"]));
    }
    static JsType fromCpp(const CppType& c)
    {
        em::val js = em::val::object();
        js.set("_x", djinni::I32::Boxed::fromCpp(c._x));
        js.set("_y", djinni::String::Boxed::fromCpp(c._y));
        return js;
    }
};

struct NativeMyInterface : djinni::JsInterface<MyInterface, NativeMyInterface> {
    class JsProxy: public djinni::JsProxyBase,
                   public MyInterface,
                   public djinni::InstanceTracker<JsProxy> {
    public:
        JsProxy(const em::val& v) : JsProxyBase(v) {}
        void foo(int x) override {
            return _jsRef().call<void>("foo", x);
        }
        std::wstring testStr(const std::wstring& x) override {
            return djinni::WString::toCpp(_jsRef().call<std::wstring>("testStr", djinni::WString::fromCpp(x)));
        }
        std::vector<uint8_t> testBin(const std::vector<uint8_t>& bin) override {
            return djinni::Binary::toCpp(_jsRef().call<em::val>("testBin", djinni::Binary::fromCpp(bin)));
        }
        std::chrono::system_clock::time_point testDate(const std::chrono::system_clock::time_point& d) override {
            return djinni::Date::toCpp(_jsRef().call<em::val>("testDate", djinni::Date::fromCpp(d)));
        }
        MyRecord testRecord(const MyRecord& r) override {
            return NativeMyRecord::toCpp(_jsRef().call<em::val>("testRecord", NativeMyRecord::fromCpp(r)));
        }
        std::optional<MyRecord> testOptional1(const std::optional<MyRecord>& o) override {
            return djinni::Optional<std::optional, NativeMyRecord>::toCpp(
                _jsRef().call<em::val>("testOptional1",
                                   djinni::Optional<std::optional, NativeMyRecord>::fromCpp(o)));
        }
        std::shared_ptr<MyInterface> testOptional2(const std::shared_ptr<MyInterface>& o) override {
            return djinni::Optional<std::optional, NativeMyInterface>::toCpp(
                _jsRef().call<em::val>("testOptional2",
                                   djinni::Optional<std::optional, NativeMyInterface>::fromCpp(o)));
        }
        std::vector<MyRecord> testList(const std::vector<MyRecord>& l) override {
            return djinni::List<NativeMyRecord>::toCpp(_jsRef().call<em::val>("testList", djinni::List<NativeMyRecord>::fromCpp(l)));
        }
        std::unordered_set<std::string> testSet(const std::unordered_set<std::string>& s) override {
            return djinni::Set<djinni::String>::toCpp(_jsRef().call<em::val>("testSet", djinni::Set<djinni::String>::fromCpp(s)));
        }
        std::unordered_map<std::string, MyRecord> testMap(const std::unordered_map<std::string, MyRecord>& m) override {
            return djinni::Map<djinni::String, NativeMyRecord>::toCpp(_jsRef().call<em::val>("testMap", djinni::Map<djinni::String, NativeMyRecord>::fromCpp(m)));
        }
        djinni::expected<MyRecord, std::string> testOutcome(bool success) override {
            return djinni::Outcome<NativeMyRecord, djinni::String>::toCpp(_jsRef().call<em::val>("testOutcome", djinni::Bool::fromCpp(success)));
        }
    };
    static em::val cppProxy() {
        static em::val inst = em::val::module_property("MyInterface_CppProxy");
        return inst;
    }
    // ---------
    using CppType = std::shared_ptr<MyInterface>;
    using CppOptType = std::shared_ptr<MyInterface>;
    using JsType = em::val;
    using Boxed = NativeMyInterface;
    static CppType toCpp(JsType j) { return _fromJs(j); }
    static JsType fromCppOpt(const CppOptType& c) { return {_toJs(c)}; }
    static JsType fromCpp(const CppType& c) { return fromCppOpt(c); }
    // ---------
    static void foo(const std::shared_ptr<MyInterface>& self, int32_t x) {
        return self->foo(djinni::I32::toCpp(x));
    }
    static std::wstring testStr(const std::shared_ptr<MyInterface>& self, const std::wstring& x) {
        return djinni::WString::fromCpp(self->testStr(djinni::WString::toCpp(x)));
    }
    static em::val testBin(const std::shared_ptr<MyInterface>& self, const em::val& bin) {
        return djinni::Binary::fromCpp(self->testBin(djinni::Binary::toCpp(bin)));
    }
    static em::val testDate(const std::shared_ptr<MyInterface>& self, const em::val& d) {
        return djinni::Date::fromCpp(self->testDate(djinni::Date::toCpp(d)));
    }
    static em::val testRecord(const std::shared_ptr<MyInterface>& self, const em::val& r) {
        return NativeMyRecord::fromCpp(self->testRecord(NativeMyRecord::toCpp(r)));
    }
    static em::val testOptional1(const std::shared_ptr<MyInterface>& self, const em::val& o) {
        return djinni::Optional<std::optional, NativeMyRecord>::fromCpp(self->testOptional1(djinni::Optional<std::optional, NativeMyRecord>::toCpp(o)));
    }
    static em::val testOptional2(const std::shared_ptr<MyInterface>& self, const em::val& o) {
        return djinni::Optional<std::optional, NativeMyInterface>::fromCpp(self->testOptional2(djinni::Optional<std::optional, NativeMyInterface>::toCpp(o)));
    }
    static em::val testList(const std::shared_ptr<MyInterface>& self, const em::val& l) {
        return djinni::List<NativeMyRecord>::fromCpp(self->testList(djinni::List<NativeMyRecord>::toCpp(l)));
    }
    static em::val testSet(const std::shared_ptr<MyInterface>& self, const em::val& s) {
        return djinni::Set<djinni::String>::fromCpp(self->testSet(djinni::Set<djinni::String>::toCpp(s)));
    }
    static em::val testMap(const std::shared_ptr<MyInterface>& self, const em::val& m) {
        return djinni::Map<djinni::String, NativeMyRecord>::fromCpp(self->testMap(djinni::Map<djinni::String, NativeMyRecord>::toCpp(m)));
    }
    static em::val testOutcome(const std::shared_ptr<MyInterface>& self, bool success) {
        return djinni::Outcome<NativeMyRecord, djinni::String>::fromCpp(self->testOutcome(djinni::Bool::toCpp(success)));
    }
    static em::val create() {
        return NativeMyInterface::fromCpp(MyInterface::create());
    }
    static em::val instance() {
        return NativeMyInterface::fromCpp(MyInterface::instance());
    }
    static em::val pass(const em::val& i) {
        return NativeMyInterface::fromCpp(MyInterface::pass(NativeMyInterface::toCpp(i)));
    }
    static bool comp(const em::val& i, const em::val& j) {
        return djinni::Bool::fromCpp(MyInterface::comp(NativeMyInterface::toCpp(i), NativeMyInterface::toCpp(j)));
    }
    static void testOutcome2(const em::val& i) {
        return MyInterface::testOutcome2(NativeMyInterface::toCpp(i));
    }
    static em::val testArray(const em::val& a) {
        return djinni::Array<djinni::I32>::fromCpp(MyInterface::testArray(djinni::Array<djinni::I32>::toCpp(a)));
    }
};

// djinni generated binding
EMSCRIPTEN_BINDINGS(MyInterface) {
    em::class_<MyInterface>("MyInterface")
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

}

// user implementation
class MyInterfaceImpl : public MyInterface, public djinni::InstanceTracker<MyInterfaceImpl> {
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
    djinni::expected<MyRecord, std::string> testOutcome(bool success) override {
        if (success) {
            return MyRecord(100, "hello");
        } else {
            return djinni::make_unexpected("BAD");
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

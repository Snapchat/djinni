#include "test_helpers.hpp"
#include "client_returned_record.hpp"
#include "client_interface.hpp"
#include "async_interface.hpp"
#include "user_token.hpp"
#include "assorted_primitives.hpp"
#include "color.hpp"
#include "map_list_record.hpp"
#include "nested_collection.hpp"
#include "primitive_list.hpp"
#include "set_record.hpp"
#include <exception>

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#else
#include <thread>
#endif

namespace testsuite {

SetRecord TestHelpers::get_set_record() {
    return SetRecord { {
        "StringA",
        "StringB",
        "StringC"
    }, std::unordered_set<int32_t>{} };
}

bool TestHelpers::check_set_record(const SetRecord & rec) {
    return rec.set == std::unordered_set<std::string>{ "StringA", "StringB", "StringC" };
}

static const PrimitiveList cPrimitiveList { { 1, 2, 3 } };

PrimitiveList TestHelpers::get_primitive_list() {
    return cPrimitiveList;
}

bool TestHelpers::check_primitive_list(const PrimitiveList & pl) {
    return pl.list == cPrimitiveList.list;
}

static const NestedCollection cNestedCollection { { {u8"String1", u8"String2"},
                                                    {u8"StringA", u8"StringB"} } };

NestedCollection TestHelpers::get_nested_collection() {
    return cNestedCollection;
}

bool TestHelpers::check_nested_collection(const NestedCollection & nc) {
    return nc.set_list == cNestedCollection.set_list;
}

static const std::unordered_map<std::string, int64_t> cMap = {
    { "String1", 1 },
    { "String2", 2 },
    { "String3", 3 },
};

std::unordered_map<std::string, int64_t> TestHelpers::get_map() {
    return cMap;
}

bool TestHelpers::check_map(const std::unordered_map<std::string, int64_t> & m) {
    return m == cMap;
}

std::unordered_map<std::string, int64_t> TestHelpers::get_empty_map() {
    return std::unordered_map<std::string, int64_t>();
}

bool TestHelpers::check_empty_map(const std::unordered_map<std::string,int64_t> & m) {
    return m.empty();
}

MapListRecord TestHelpers::get_map_list_record() {
    return { { cMap } };
}

bool TestHelpers::check_map_list_record(const MapListRecord & rec) {
    return rec.map_list.size() == 1 && rec.map_list[0] == cMap;
}

static const std::string HELLO_WORLD = "Hello World!";
static const std::string NON_ASCII("Non-ASCII /\0 非 ASCII 字符",
                                   sizeof("Non-ASCII /\0 非 ASCII 字符")-1);

void TestHelpers::check_client_interface_ascii(const std::shared_ptr<ClientInterface> & i) {
    ClientReturnedRecord cReturnedRecord = i->get_record(5, HELLO_WORLD, {});
    if (cReturnedRecord.content != HELLO_WORLD) {
        std::string error_msg = "Expected String: " + HELLO_WORLD + " Actual: " + cReturnedRecord.content;
        throw std::invalid_argument(error_msg);
    }
}

void TestHelpers::check_client_interface_nonascii(const std::shared_ptr<ClientInterface> & i) {
    ClientReturnedRecord cReturnedRecord = i->get_record(5, NON_ASCII, {});
    if (cReturnedRecord.content != NON_ASCII) {
        std::string error_msg = "Expected String: " + NON_ASCII + " Actual: " + cReturnedRecord.content;
        throw std::invalid_argument(error_msg);
    }
}

void TestHelpers::check_client_interface_args(const std::shared_ptr<ClientInterface> & i) {
    const std::string returned1 = i->meth_taking_interface(i);
    if ("test" != returned1) {
        std::string error_msg = "Expected String: 'test' Actual: '" + returned1 + "'";
        throw std::invalid_argument(error_msg);
    }

    const std::string returned2 = i->meth_taking_optional_interface(i);
    if ("test" != returned2) {
        std::string error_msg = "Expected String: 'test' Actual: '" + returned2 + "'";
        throw std::invalid_argument(error_msg);
    }
}

std::shared_ptr<UserToken> TestHelpers::token_id(const std::shared_ptr<UserToken> & in) {
    return in;
}

class CppToken : public UserToken {
    std::string whoami() { return "C++"; }
};

std::shared_ptr<UserToken> TestHelpers::create_cpp_token() {
    return std::make_shared<CppToken>();
}

void TestHelpers::check_cpp_token(const std::shared_ptr<UserToken> & in) {
    // Throws bad_cast if type is wrong
    (void)dynamic_cast<CppToken &>(*in);
}

int64_t TestHelpers::cpp_token_id(const std::shared_ptr<UserToken> & in) {
    return reinterpret_cast<int64_t>(in.get());
}

void TestHelpers::check_token_type(const std::shared_ptr<UserToken> &t, const std::string & type) {
    if (t->whoami() != type) {
        throw std::invalid_argument("wrong token type");
    }
}

std::experimental::optional<int32_t> TestHelpers::return_none() {
    return {};
}

void TestHelpers::check_enum_map(const std::unordered_map<color, std::string> & m) {
    std::unordered_map<color, std::string> expected;
    // test the to_string conversion here
    for (auto c: {color::RED, color::ORANGE, color::YELLOW, color::GREEN, color::BLUE, color::INDIGO, color::VIOLET}) {
        expected[c] = to_string(c);
    }
    if (m != expected) {
        throw std::invalid_argument("map mismatch");
    }
}

void TestHelpers::check_enum(color) {} // stub

AssortedPrimitives TestHelpers::assorted_primitives_id(const AssortedPrimitives & p) {
    return p;
}

std::vector<uint8_t> TestHelpers::id_binary(const std::vector<uint8_t> & v) {
    return v;
}

djinni::Future<int32_t> TestHelpers::get_async_result() {
    auto* p = new djinni::Promise<int32_t>();
    auto f = p->getFuture();

#if defined(__EMSCRIPTEN__)
    emscripten_async_call([] (void* context) {
        auto* p = reinterpret_cast<djinni::Promise<int32_t>*>(context);
        p->setValue(42);
        delete p;
    }, p, 10/*ms*/);
#else
    std::thread t([p] () mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        p->setValue(42);
        delete p;
    });
    t.detach();
#endif
    return f;
}

djinni::Future<int32_t> TestHelpers::async_early_throw() {
    throw std::runtime_error("error");
}

djinni::Future<std::string> TestHelpers::future_roundtrip(djinni::Future<int32_t> f) {
#ifdef DJINNI_FUTURE_HAS_COROUTINE_SUPPORT
    auto i = co_await f;
    co_return std::to_string(i);
#else
    return f.then([] (djinni::Future<int32_t> f) {
        return std::to_string(f.get());
    });
#endif
}

djinni::Future<std::string> TestHelpers::check_async_interface(const std::shared_ptr<AsyncInterface> & i) {
#ifdef DJINNI_FUTURE_HAS_COROUTINE_SUPPORT
    auto f1 = [] () -> djinni::Future<std::string> {
        co_return "36";
    };
    auto f2 = [] (auto f1) -> djinni::Future<int> {
        co_return std::stoi(co_await f1);
    };
    auto f3 = i->future_roundtrip(f2(f1()));
    return f3;
#else
    djinni::Promise<std::string> p;
    auto f = p.getFuture();
    auto f2 = f.then([] (djinni::Future<std::string> s) {
        return std::stoi(s.get());
    });
    auto f3 = i->future_roundtrip(std::move(f2));
    p.setValue("36");
    return f3;
#endif
}

djinni::Future<std::string> TestHelpers::check_async_composition(const std::shared_ptr<AsyncInterface> & i) {
    djinni::Promise<std::string> p1;
    djinni::Promise<std::string> p2;
    auto f1 = p1.getFuture();
    auto f2 = p2.getFuture();

    auto str2num = [] (djinni::Future<std::string> s) {
        return std::stoi(s.get());
    };
    
    auto f3 = f1.then(str2num);
    auto f4 = f2.then(str2num);
    
    std::vector<djinni::Future<std::string>> futures;
    futures.push_back(i->future_roundtrip(std::move(f3)));
    futures.push_back(i->future_roundtrip(std::move(f4)));

    p1.setValue("36");
    p2.setValue("36");
    
#ifdef DJINNI_FUTURE_HAS_COROUTINE_SUPPORT
    co_await djinni::whenAll(futures);
    co_return std::string("42");
#else
    return djinni::whenAll(futures).then([] (auto f) {
        return std::string("42");
    });
#endif
}

::djinni::Future<void> TestHelpers::void_async_method(djinni::Future<void> f) {
#ifdef DJINNI_FUTURE_HAS_COROUTINE_SUPPORT
    co_return co_await f;
#else
    return f;
#endif
}

std::vector<std::experimental::optional<std::string>> TestHelpers::get_optional_list() {
    return {std::experimental::nullopt, std::string("hello")};
}

bool TestHelpers::check_optional_list(const std::vector<std::experimental::optional<std::string>> & ol) {
    return ol.size() == 2 &&
        ol[0] == std::experimental::nullopt &&
        ol[1] == std::string("hello");
}

std::unordered_set<std::experimental::optional<std::string>> TestHelpers::get_optional_set() {
    return {std::experimental::nullopt, std::string("hello")};
}

bool TestHelpers::check_optional_set(const std::unordered_set<std::experimental::optional<std::string>> & os) {
    return os.size() == 2 &&
        os.find(std::experimental::nullopt) != os.end() &&
        os.find(std::string("hello")) != os.end();
}

std::unordered_map<std::experimental::optional<std::string>, std::experimental::optional<std::string>> TestHelpers::get_optional_map() {
    return {{std::experimental::nullopt, std::string("hello")}, {std::string("hello"), std::experimental::nullopt}};
}

bool TestHelpers::check_optional_map(const std::unordered_map<std::experimental::optional<std::string>, std::experimental::optional<std::string>> & om) {
    auto i1 = om.find(std::experimental::nullopt);
    auto i2 = om.find(std::string("hello"));
    return om.size() == 2 &&
        i1->second == std::string("hello") &&
        i2->second == std::experimental::nullopt;
}

} // namespace testsuite

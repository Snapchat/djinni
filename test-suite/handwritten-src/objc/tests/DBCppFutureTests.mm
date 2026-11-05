#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#include <Future.hpp>

#include <memory>
#include <optional>
#include <string>

#ifdef DJINNI_FUTURE_HAS_COROUTINE_SUPPORT

namespace {

template<typename Functor>
struct OnCleanup {
    OnCleanup(Functor&& functor)
    :functor{std::move(functor)}
    {}
    ~OnCleanup() {
        functor();
    }
    Functor functor;
};

djinni::Future<void> inner_coroutine(std::vector<int>& cleanup_ids, int id, djinni::Future<void> suspendOn) {
    OnCleanup cleanup{
        [&] {
            cleanup_ids.push_back(id);
        }
    };

    co_await suspendOn;
    co_return; // do not remove!
}

djinni::Future<void> outer_coroutine(std::vector<int>& cleanup_ids, int id, djinni::Future<void> suspendOn) {
    OnCleanup cleanup{
        [&] {
            cleanup_ids.push_back(id);
        }
    };

    co_await inner_coroutine(cleanup_ids, id + 1, std::move(suspendOn));
    co_return; // do not remove!
}

}

#endif

@interface DBCppFutureTests : XCTestCase
@end

@implementation DBCppFutureTests

#ifdef DJINNI_FUTURE_HAS_COROUTINE_SUPPORT
- (void) testFutureCoroutines_cleanupOrder {
    std::vector<int> cleanupIds{};

    djinni::Promise<void> djinniPromise{};

    auto coroutineFuture = outer_coroutine(cleanupIds, 0, djinniPromise.getFuture());
    XCTAssertFalse(coroutineFuture.isReady());

    djinniPromise.setValue();
    XCTAssertTrue(coroutineFuture.isReady());

    XCTAssertEqual(cleanupIds.size(), 2);
    XCTAssertEqual(cleanupIds[0], 1); // the inner coroutine should be cleaned up first
    XCTAssertEqual(cleanupIds[1], 0); // ... then the outer
}
#endif

- (void) testFuture_brokenPromise {
    std::optional<djinni::Promise<void>> promise{std::in_place};
    auto future = promise->getFuture();
    XCTAssertFalse(future.isReady());

    auto promise2 = std::make_optional(std::move(*promise));
    XCTAssertFalse(future.isReady());

    promise.reset();
    XCTAssertFalse(future.isReady());

    promise2.reset();
    XCTAssertTrue(future.isReady());
    if (future.isReady()) {
        XCTAssertThrowsSpecific(future.get(), djinni::BrokenPromiseException);
    }
}

- (void) testFuture_brokenPromiseAssignment {
    djinni::Promise<void> promise{};
    auto future = promise.getFuture();
    XCTAssertFalse(future.isReady());

    promise = djinni::Promise<void>{};
    XCTAssertTrue(future.isReady());
    if (future.isReady()) {
        XCTAssertThrowsSpecific(future.get(), djinni::BrokenPromiseException);
    }

    XCTAssertFalse(promise.getFuture().isReady());
}

- (void) testFutureThen_unwrapsPendingFuture {
    djinni::Promise<int> outer;
    djinni::Promise<int> inner;
    auto innerFuture = std::make_optional(inner.getFuture());

    auto chained = outer.getFuture().then([&] (djinni::Future<int> f) {
        XCTAssertEqual(f.get(), 1);
        return std::move(*innerFuture);
    });
    static_assert(std::is_same_v<decltype(chained), djinni::Future<int>>);

    XCTAssertFalse(chained.isReady());
    outer.setValue(1);
    // handler has run, but the future it returned is still pending
    XCTAssertFalse(chained.isReady());
    inner.setValue(42);
    XCTAssertTrue(chained.isReady());
    XCTAssertEqual(chained.get(), 42);
}

- (void) testFutureThen_unwrapsReadyFuture {
    auto chained = djinni::Promise<int>::resolve(20).then([] (djinni::Future<int> f) {
        return djinni::Promise<std::string>::resolve(std::to_string(f.get() + 1));
    });
    static_assert(std::is_same_v<decltype(chained), djinni::Future<std::string>>);
    XCTAssertTrue(chained.isReady());
    XCTAssertEqual(chained.get(), std::string("21"));
}

- (void) testFutureThen_mixesSyncAndAsyncHandlers {
    auto doubleAsync = [] (djinni::Future<int> f) {
        return djinni::Promise<int>::resolve(f.get() * 2);
    };
    auto doubleSync = [] (djinni::Future<int> f) {
        return f.get() * 2;
    };
    djinni::Promise<int> p;
    auto result = p.getFuture().then(doubleAsync).then(doubleSync).then(doubleAsync);
    static_assert(std::is_same_v<decltype(result), djinni::Future<int>>);
    p.setValue(4);
    XCTAssertEqual(result.get(), 32);
}

- (void) testFutureThen_unwrapsVoidFuture {
    djinni::Promise<void> inner;
    auto innerFuture = std::make_optional(inner.getFuture());
    auto chained = djinni::Promise<int>::resolve(1).then([&] (djinni::Future<int>) {
        return std::move(*innerFuture);
    });
    static_assert(std::is_same_v<decltype(chained), djinni::Future<void>>);
    XCTAssertFalse(chained.isReady());
    inner.setValue();
    XCTAssertTrue(chained.isReady());
    XCTAssertNoThrow(chained.get());
}

- (void) testFutureThen_unwrapsMoveOnlyValue {
    auto chained = djinni::Promise<int>::resolve(7).then([] (djinni::Future<int> f) {
        return djinni::Promise<std::unique_ptr<int>>::resolve(std::make_unique<int>(f.get()));
    });
    static_assert(std::is_same_v<decltype(chained), djinni::Future<std::unique_ptr<int>>>);
    auto ptr = chained.get();
    XCTAssertTrue(ptr != nullptr);
    XCTAssertEqual(*ptr, 7);
}

- (void) testFutureThen_propagatesInnerException {
    djinni::Promise<int> inner;
    auto innerFuture = std::make_optional(inner.getFuture());
    auto chained = djinni::Promise<int>::resolve(1).then([&] (djinni::Future<int>) {
        return std::move(*innerFuture);
    });
    inner.setException(std::runtime_error("inner failed"));
    XCTAssertTrue(chained.isReady());
    XCTAssertThrowsSpecific(chained.get(), std::runtime_error);
}

- (void) testFutureThen_propagatesHandlerException {
    auto chained = djinni::Promise<int>::resolve(1).then([] (djinni::Future<int>) -> djinni::Future<int> {
        throw std::runtime_error("handler failed");
    });
    XCTAssertTrue(chained.isReady());
    XCTAssertThrowsSpecific(chained.get(), std::runtime_error);
}

- (void) testFutureThen_propagatesOuterExceptionThroughUnwrap {
    auto chained = djinni::Promise<int>::reject(std::make_exception_ptr(std::runtime_error("outer failed"))).then([] (djinni::Future<int> f) {
        return djinni::Promise<int>::resolve(f.get());
    });
    XCTAssertThrowsSpecific(chained.get(), std::runtime_error);
}

- (void) testFutureThen_brokenInnerPromise {
    std::optional<djinni::Promise<int>> inner{std::in_place};
    auto innerFuture = std::make_optional(inner->getFuture());
    auto chained = djinni::Promise<int>::resolve(1).then([&] (djinni::Future<int>) {
        return std::move(*innerFuture);
    });
    XCTAssertFalse(chained.isReady());
    inner.reset();
    XCTAssertTrue(chained.isReady());
    XCTAssertThrowsSpecific(chained.get(), djinni::BrokenPromiseException);
}

- (void) testFutureThen_nonFutureResultTypesUnchanged {
    auto intFuture = djinni::Promise<int>::resolve(1).then([] (djinni::Future<int> f) { return f.get(); });
    static_assert(std::is_same_v<decltype(intFuture), djinni::Future<int>>);
    auto voidFuture = djinni::Promise<int>::resolve(1).then([] (djinni::Future<int>) {});
    static_assert(std::is_same_v<decltype(voidFuture), djinni::Future<void>>);
    XCTAssertEqual(intFuture.get(), 1);
    XCTAssertNoThrow(voidFuture.get());
}

#ifdef DJINNI_FUTURE_HAS_COROUTINE_SUPPORT
- (void) testFutureThen_unwrapsCoroutine {
    djinni::Promise<int> p;
    auto chained = p.getFuture().then([] (djinni::Future<int> f) -> djinni::Future<int> {
        co_return co_await djinni::Promise<int>::resolve(f.get() + 1);
    });
    static_assert(std::is_same_v<decltype(chained), djinni::Future<int>>);
    p.setValue(41);
    XCTAssertEqual(chained.get(), 42);
}
#endif

@end

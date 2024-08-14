#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#include <Future.hpp>

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

@end

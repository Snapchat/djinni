#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#include "../../../support-lib/cpp/SharedFuture.hpp"

@interface DBSharedFutureTest : XCTestCase
@end

@implementation DBSharedFutureTest

#ifdef DJINNI_FUTURE_HAS_COROUTINE_SUPPORT

- (void)setUp
{
    [super setUp];
}

- (void)tearDown
{
    [super tearDown];
}

- (void)testCreateFuture
{
    djinni::SharedFuture<int> resolvedInt(djinni::Promise<int>::resolve(42));
    XCTAssertEqual(resolvedInt.get(), 42);

    djinni::Promise<NSString*> strPromise;
    djinni::SharedFuture futureString(strPromise.getFuture());

    strPromise.setValue(@"foo");
    XCTAssertEqualObjects(futureString.get(), @"foo");
}

- (void)testThen
{
    djinni::Promise<int> intPromise;
    djinni::SharedFuture<int> futureInt(intPromise.getFuture());

    auto transformedInt = futureInt.thenShared([](const auto& resolved) { return 2 * resolved.get(); });

    intPromise.setValue(42);
    XCTAssertEqual(transformedInt.get(), 84);

    // Also verify multiple consumers and chaining.
    auto transformedString = futureInt.thenShared([](const auto& resolved) { return std::to_string(resolved.get()); });
    auto futurePlusOneTimesTwo = futureInt.then([](auto resolved) { return resolved.get() + 1; }).then([](auto resolved) {
        return 2 * resolved.get();
    });
    auto futureStringLen = transformedString.then([](auto resolved) { return resolved.get().length(); });

    XCTAssertEqual(transformedString.get(), std::string("42"));
    XCTAssertEqual(futurePlusOneTimesTwo.get(), (42 + 1) * 2);
    XCTAssertEqual(futureStringLen.get(), 2);

    XCTAssertEqual(futureInt.get(), 42);

    auto voidFuture = transformedString.thenShared([](auto) {});
    voidFuture.wait();

    auto intFuture2 = voidFuture.thenShared([](auto) { return 43; });
    XCTAssertEqual(intFuture2.get(), 43);
}

- (void)testThenUnwrapsFuture
{
    djinni::Promise<int> intPromise;
    djinni::SharedFuture<int> futureInt(intPromise.getFuture());

    djinni::Promise<std::string> innerPromise;
    auto innerFuture = std::make_optional(innerPromise.getFuture());
    auto unwrapped = futureInt.then([&](const auto& resolved) {
        XCTAssertEqual(resolved.get(), 42);
        return std::move(*innerFuture);
    });
    static_assert(std::is_same_v<decltype(unwrapped), djinni::Future<std::string>>);

    auto unwrappedShared = futureInt.thenShared([](const auto& resolved) {
        return djinni::Promise<int>::resolve(resolved.get() + 1);
    });
    static_assert(std::is_same_v<decltype(unwrappedShared), djinni::SharedFuture<int>>);

    auto unwrappedVoid = futureInt.then([](const auto&) { return djinni::Promise<void>::resolve(true); });
    static_assert(std::is_same_v<decltype(unwrappedVoid), djinni::Future<void>>);

    auto unwrappedError = futureInt.then([](const auto&) {
        return djinni::Promise<int>::reject(std::runtime_error("inner failed"));
    });

    intPromise.setValue(42);
    XCTAssertEqual(unwrappedShared.get(), 43);
    XCTAssertNoThrow(unwrappedVoid.get());
    XCTAssertThrows(unwrappedError.get());
    XCTAssertFalse(unwrapped.isReady());
    innerPromise.setValue("done");
    XCTAssertEqual(unwrapped.get(), std::string("done"));
}

- (void)testException
{
    // Also verify exception handling.
    djinni::Promise<int> intPromise;
    djinni::SharedFuture<int> futureInt(intPromise.getFuture());

    intPromise.setException(std::runtime_error("mocked"));

    XCTAssertThrows(futureInt.get());

    auto thenResult = futureInt.then([](auto resolved) { return resolved.get(); });
    XCTAssertThrows(thenResult.get());

    auto withExceptionHandling = futureInt.thenShared([](const auto& resolved) {
      try {
        return resolved.get();
      } catch (...) {
        return -1;
      }
    });
    withExceptionHandling.wait();
    XCTAssertEqual(withExceptionHandling.get(), -1);
}

#endif

@end

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

    auto transformedInt = futureInt.then([](const auto& resolved) { return 2 * resolved.get(); });

    intPromise.setValue(42);
    XCTAssertEqual(transformedInt.get(), 84);

    // Also verify multiple consumers and chaining.
    auto transformedString = futureInt.then([](const auto& resolved) { return std::to_string(resolved.get()); });
    auto futurePlusOneTimesTwo = futureInt.then([](const auto& resolved) { return resolved.get() + 1; }).then([](const auto& resolved) {
        return 2 * resolved.get();
    });
    auto futureStringLen = transformedString.then([](const auto& resolved) { return resolved.get().length(); }).toFuture();

    XCTAssertEqual(transformedString.get(), std::string("42"));
    XCTAssertEqual(futurePlusOneTimesTwo.get(), (42 + 1) * 2);
    XCTAssertEqual(futureStringLen.get(), 2);

    XCTAssertEqual(futureInt.get(), 42);

    auto voidFuture = transformedString.then([]() {});
    voidFuture.wait();

    auto intFuture2 = voidFuture.then([]() { return 43; });
    XCTAssertEqual(intFuture2.get(), 43);
}

- (void)testException
{
    // Also verify exception handling.
    djinni::Promise<int> intPromise;
    djinni::SharedFuture<int> futureInt(intPromise.getFuture());

    intPromise.setException(std::runtime_error("mocked"));

    XCTAssertThrows(futureInt.get());

    auto thenResult = futureInt.then([]() { return 43; });
    XCTAssertThrows(thenResult.get());

    auto withExceptionHandling = futureInt.then([](const auto& resolved) {
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

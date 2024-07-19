#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#include "../../../support-lib/cpp/SharedFuture.hpp"

@interface DBSharedFutureTest : XCTestCase
@end

@implementation DBSharedFutureTest

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

    auto transformedInt = futureInt.then([](int i) { return 2 * i; });

    intPromise.setValue(42);
    XCTAssertEqual(transformedInt.get(), 84);

    // Also verify multiple consumers and chaining.
    auto transformedString = futureInt.then([](int i) { return std::to_string(i); });
    auto futurePlusOneTimesTwo = futureInt.then([](int i) { return i + 1; }).then([](int i) { return 2 * i; });
    auto futureStringLen = transformedString.then([](const std::string& s) { return s.length(); }).toFuture();

    XCTAssertEqual(transformedString.get(), std::string("42"));
    XCTAssertEqual(futurePlusOneTimesTwo.get(), (42 + 1) * 2);
    XCTAssertEqual(futureStringLen.get(), 2);

    XCTAssertEqual(futureInt.get(), 42);

    auto voidFuture = transformedString.then([]() {});
    voidFuture.wait();

    auto intFuture2 = voidFuture.then([]() { return 43; });
    XCTAssertEqual(intFuture2.get(), 43);
}

@end

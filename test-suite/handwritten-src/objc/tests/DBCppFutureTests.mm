#import <XCTest/XCTest.h>

#include <Future.hpp>

@interface DBCppFutureTests : XCTestCase

@end

@implementation DBCppFutureTests

- (void)setUp
{
    [super setUp];
}

- (void)tearDown
{
    [super tearDown];
}

- (void)testCppFutureMovesResult
{
    auto future = djinni::Promise<std::unique_ptr<int>>::resolve(std::make_unique<int>(5));
    auto ptr = future.get();
    XCTAssertTrue(!!ptr);
    XCTAssertEqual(*ptr, 5);
}

@end

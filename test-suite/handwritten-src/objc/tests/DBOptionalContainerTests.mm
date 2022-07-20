#import "DBTestHelpers.h"

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

// using namespace testsuite;

@interface DBOptionalContainerTests : XCTestCase

@end

@implementation DBOptionalContainerTests

- (void)setUp
{
    [super setUp];
}

- (void)tearDown
{
    [super tearDown];
}

- (void)testOptionalList
{
    NSArray<NSString *> * ol = [DBTestHelpers getOptionalList];
    XCTAssertEqual([ol count], 2);
    XCTAssertEqualObjects(ol[0], [NSNull null]);
    XCTAssertEqualObjects(ol[1], @"hello");

    BOOL ret = [DBTestHelpers checkOptionalList:ol];
    XCTAssertTrue(ret);
}

- (void)testOptionalSet
{
    NSSet<NSString *> * os = [DBTestHelpers getOptionalSet];
    // NSArray's interface doesn't allow us to directly check for NSNull, but we
    // can see the count is 2
    XCTAssertEqual([os count], 2);
    XCTAssertTrue([os containsObject:@"hello"]);

    BOOL ret = [DBTestHelpers checkOptionalSet:os];
    XCTAssertTrue(ret);
}

- (void)testOptionalMap
{
    NSDictionary<NSString *, NSString *> * om = [DBTestHelpers getOptionalMap];
    // Can't directly check for NSNull, but we will pass the same dictionary
    // into C++ and check there.
    XCTAssertEqual([om count], 2);

    BOOL ret = [DBTestHelpers checkOptionalMap:om];
    XCTAssertTrue(ret);
}

@end

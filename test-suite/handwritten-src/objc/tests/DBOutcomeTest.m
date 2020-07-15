#import <Foundation/Foundation.h>
#import <DBTestOutcome.h>
#import <DBNestedOutcome.h>
#import <XCTest/XCTest.h>

@interface DBOutcomeTests : XCTestCase
@end

@implementation DBOutcomeTests

- (void)setUp
{
    [super setUp];
}

- (void)tearDown
{
    [super tearDown];
}

- (void)test
{
    // construct result outcome in native and pass to objc
    DJOutcome<NSString*, NSNumber*>* r = [DBTestOutcome getSuccessOutcome];
    // results are equal
    XCTAssertEqualObjects([r result], @"hello");
    // outcome objects compare equal
    XCTAssertEqualObjects(r, [DJOutcome fromResult:@"hello"]);

    // construct error outcome in native and pass to objc
    DJOutcome<NSString*, NSNumber*>* e = [DBTestOutcome getErrorOutcome];
    // error values are equal
    XCTAssertEqualObjects([e error], @42);
    // outcome objects compare equal
    XCTAssertEqualObjects(e, [DJOutcome fromError:@42]);

    // result outcome and error outcome does not compare equal
    XCTAssertNotEqualObjects(r, e);

    // construct result outcome in objc then pass to native and back
    XCTAssertEqualObjects([DBTestOutcome putSuccessOutcome:[DJOutcome fromResult:@"hello"]], @"hello");
    // construct error outcome in objc then pass to native and back
    XCTAssertEqual([DBTestOutcome putErrorOutcome:[DJOutcome fromError:@42]], 42);

    // Hash equal
    XCTAssertEqual(r.hash, [DJOutcome fromResult:@"hello"].hash);
    XCTAssertEqual(e.hash, [DJOutcome fromError:@42].hash);
    // Hash not equal between result and error
    XCTAssertNotEqual(r.hash, [DJOutcome fromError:@"hello"].hash);
    XCTAssertNotEqual(e.hash, [DJOutcome fromResult:@42].hash);

    // test outcome as nested object
    DBNestedOutcome* nr = [DBTestOutcome getNestedSuccessOutcome];
    XCTAssertEqualObjects([nr.o result], @42);
    XCTAssertEqualObjects(nr, [DBNestedOutcome nestedOutcomeWithO:[DJOutcome fromResult:@42]]);

    DBNestedOutcome* ne = [DBTestOutcome getNestedErrorOutcome];
    XCTAssertEqualObjects([ne.o error], @"hello");
    XCTAssertEqualObjects(ne, [DBNestedOutcome nestedOutcomeWithO:[DJOutcome fromError:@"hello"]]);

    XCTAssertEqual(
        [DBTestOutcome putNestedSuccessOutcome:[DBNestedOutcome nestedOutcomeWithO:[DJOutcome fromResult:@42]]], 42);
    XCTAssertEqualObjects(
        [DBTestOutcome putNestedErrorOutcome:[DBNestedOutcome nestedOutcomeWithO:[DJOutcome fromError:@"hello"]]],
        @"hello");
}

@end

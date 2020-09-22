#import <Foundation/Foundation.h>
#import <DBTestArray.h>
#import <DBVec2.h>
#import <XCTest/XCTest.h>

@interface DBArrayTests : XCTestCase
@end

@implementation DBArrayTests

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
    NSArray<NSString*>* retStrings = [DBTestArray testStringArray: @[@"1", @"2", @"3"]];
    NSArray* expectedStrings = @[@"3", @"2", @"1"];
    XCTAssertEqualObjects(retStrings, expectedStrings);
    
    NSArray<NSNumber*>* retInts = [DBTestArray testIntArray: @[@1, @2, @3]];
    NSArray* expectedInts = @[@3, @2, @1];
    XCTAssertEqualObjects(retInts, expectedInts);
    
    NSArray<DBVec2*>* retRecords = [DBTestArray testRecordArray: @[[DBVec2 vec2WithX:1 y:1], [DBVec2 vec2WithX:2 y:2], [DBVec2 vec2WithX:3 y:3]]];
    NSArray* expectedRecords = @[[DBVec2 vec2WithX:3 y:3], [DBVec2 vec2WithX:2 y:2], [DBVec2 vec2WithX:1 y:1]];
    XCTAssertEqualObjects(retRecords, expectedRecords);
}

@end

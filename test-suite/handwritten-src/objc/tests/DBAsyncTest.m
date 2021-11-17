#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>
#import <DJFuture.h>
#import "DBTestHelpers.h"

@interface DBAsyncTests : XCTestCase
@end

@implementation DBAsyncTests

- (void)setUp
{
    [super setUp];
}

- (void)tearDown
{
    [super tearDown];
}

- (void)testConsumeNativeFuture {
    DJFuture<NSNumber*>* f1 = [DBTestHelpers getAsyncResult];
    DJFuture<NSString*>* f2 =[f1 then:^id(NSNumber* res) {
            return [res stringValue];
        }];
    [f2 then:^id(NSString* res) {
            return @([res integerValue]);
        }];
    XCTAssertEqual([[f2 get] integerValue], 42);
}

- (void) testFutureRoundtrip {
    DJPromise<NSString*>* p = [[DJPromise alloc] init];
    DJFuture<NSString*>* f = [p getFuture];
    DJFuture<NSNumber*>* f2 = [f then:^id(NSString* s) {
            return @([s integerValue]);
        }];
    DJFuture<NSString*>* f3 = [DBTestHelpers futureRoundtrip:f2];
    [p setValue:@"36"];
    XCTAssertEqualObjects([f3 get], @"36");
}

@end

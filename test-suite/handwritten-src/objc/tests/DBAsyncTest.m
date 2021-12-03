#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>
#import <DJFuture.h>
#import "DBTestHelpers.h"
#import "DBAsyncInterface.h"

@interface AsyncInterfaceImpl : NSObject <DBAsyncInterface>
@end

@implementation AsyncInterfaceImpl
- (DJFuture<NSString *> *)futureRoundtrip:(DJFuture<NSNumber *> *)f {
    return [f then:^id(DJFuture<NSNumber*>* i) { return [[i get] stringValue]; }];
}
@end

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
    DJFuture<NSString*>* f2 =[f1 then:^id(DJFuture<NSNumber*>* res) {
            return [[res get] stringValue];
        }];
    DJFuture<NSNumber*>* f3 = [f2 then:^id(DJFuture<NSString*>* res) {
            return @([[res get] integerValue]);
        }];
    NSString* s = nil;
    @try {
        NSNumber* i = [f3 get];
        XCTAssertEqual([i integerValue], 42);
    } @catch (NSException* e) {
        s = e.reason;
    }
    XCTAssertEqualObjects(s, @"123");
}

- (void) testFutureRoundtrip {
    DJPromise<NSString*>* p = [[DJPromise alloc] init];
    DJFuture<NSString*>* f = [p getFuture];
    DJFuture<NSNumber*>* f2 = [f then:^id(DJFuture<NSString*>* s) {
            if (1) {
                [NSException raise:@"djinni_error" format:@"123"];
            }
            return @([[s get] integerValue]);
        }];
    DJFuture<NSString*>* f3 = [DBTestHelpers futureRoundtrip:f2];
    [p setValue:@"36"];
    NSString* s = nil;
    @try {
        NSString* i = [f3 get];
        XCTAssertEqualObjects(i, @"36");
    } @catch (NSException* e) {
        s = e.reason;
    }
    XCTAssertEqualObjects(s, @"123");
}

- (void) testFutureRoundtripBackwards {
    DJFuture<NSString*>* s = [DBTestHelpers checkAsyncInterface: [[AsyncInterfaceImpl alloc] init]];
    XCTAssertEqualObjects([s get], @"36");
}

@end

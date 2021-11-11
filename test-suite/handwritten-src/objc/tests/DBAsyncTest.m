#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>
#import <DJFuture.h>

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

- (void)test
{
    DJPromise<NSNumber*>* promise = [[DJPromise alloc] init];
    DJFuture<NSNumber*>* future = [promise getFuture];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            [promise setValue:@42];
        });

    DJFuture<NSString*>* f = [future then:^id(NSNumber* res) {
            return [res stringValue];
        }];
    [f then:^id(NSString* res) {
            return @([res integerValue]);
        }];
    XCTAssertEqual([[f get] integerValue], 42);
}

@end

#include "DBDataRefTest.h"
#import <XCTest/XCTest.h>

@interface DBDataRefTests : XCTestCase
@end

@implementation DBDataRefTests {
    DBDataRefTest* test;
}

- (void)setUp
{
    [super setUp];
    test = [DBDataRefTest create];
}

- (void)tearDown
{
    test = nil;
    [super tearDown];
}

- (void) testSend {
    // create data in objc
    const uint8 buf[] = {0, 1, 2, 3};
    NSData* input = [NSData dataWithBytes:buf length:sizeof(buf)];
    // pass to c++
    [test sendData:input];
    NSData* output = [test retriveAsBin];
    XCTAssertEqualObjects(input, output);
}

- (void) testSendMutable {
    const uint8 buf[] = {0, 1, 2, 3};
    const uint8 reversed[] = {3, 2, 1, 0};
    NSMutableData* input = [NSMutableData dataWithBytes:buf length:sizeof(buf)];
    NSMutableData* expected = [NSMutableData dataWithBytes:reversed length:sizeof(reversed)];
    [test sendMutableData:input];
    // input buffer is modified in c++
    XCTAssertEqualObjects(input, expected);
}

- (void) testGenerateData {
    const uint8 buf[] = {0, 1, 2, 3};
    NSData* expected = [NSData dataWithBytes:buf length:sizeof(buf)];
    NSData* output = [test generateData];
    XCTAssertEqualObjects(output, expected);
}

- (void) testDataFromVec {
    const uint8 buf[] = {0, 1, 2, 3};
    NSData* expected = [NSData dataWithBytes:buf length:sizeof(buf)];
    NSData* output = [test dataFromVec];
    XCTAssertEqualObjects(output, expected);
}

- (void) testDataFromStr {
    const uint8 buf[] = {'a', 'b', 'c', 'd'};
    NSData* expected = [NSData dataWithBytes:buf length:sizeof(buf)];
    NSData* output = [test dataFromStr];
    XCTAssertEqualObjects(output, expected);
}

- (void) testSendDataView {
    const uint8 buf[] = {0, 1, 2, 3};
    NSData* input = [NSData dataWithBytes:buf length:sizeof(buf)];
    NSData* output = [test sendDataView:input];
    XCTAssertEqualObjects(input, output);
}

- (void) testRecvDataView {
    const uint8 buf[] = {0, 1, 2, 3};
    NSData* expected = [NSData dataWithBytes:buf length:sizeof(buf)];
    NSData* output = [test recvDataView];
    XCTAssertEqualObjects(output, expected);
}

@end

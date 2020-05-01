#import "DBFunctionPrologueHelper.h"
#import <XCTest/XCTest.h>

@interface DBFunctionPrologueTests : XCTestCase

@end

@implementation DBFunctionPrologueTests

- (void)testFoo {
    XCTAssertEqualObjects([DBFunctionPrologueHelper foo], @"FunctionPrologueHelper.foo");
}

@end

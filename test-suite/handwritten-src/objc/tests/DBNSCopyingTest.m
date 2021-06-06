#import <Foundation/Foundation.h>
#import "DBSupportCopying.h"
#import <XCTest/XCTest.h>

@interface DBNSCopyingTest : XCTestCase

@end

@implementation DBNSCopyingTest

- (void)test
{
    DBSupportCopying* object = [DBSupportCopying supportCopyingWithX:4321];
    NSObject<NSCopying>* copyable = (NSObject<NSCopying>*) object;
    DBSupportCopying* copy = [copyable copyWithZone:nil];

    XCTAssertEqualObjects(object, copy);
}

@end

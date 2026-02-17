#import <Foundation/Foundation.h>
#import <DBTestProvider.h>
#import <DBNestedProvider.h>
#import <DBNestedProviderInterface.h>
#import <DBSimpleObject.h>
#import <XCTest/XCTest.h>

@interface DBProviderTests : XCTestCase
@end

// Simple memoization helper - caches the result of a block
static id (^memoize(id (^factory)(void)))(void) {
    __block id cachedValue = nil;
    __block BOOL hasValue = NO;
    __block NSLock *lock = [[NSLock alloc] init];

    return ^id() {
        [lock lock];
        if (!hasValue) {
            cachedValue = factory();
            hasValue = YES;
        }
        [lock unlock];
        return cachedValue;
    };
}

@implementation DBProviderTests

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
    DJProvider<NSString *> *providerStr = [DBTestProvider getProviderString];
    XCTAssertEqualObjects([providerStr get], @"hello");

    DJProvider<NSNumber *> *providerInt = [DBTestProvider getProviderInt];
    XCTAssertEqual([[providerInt get] intValue], 42);

    DJProvider<NSString *> *objcProviderStr = [DJProvider providerWithBlock:^NSString*() {
        return @"world";
    }];
    XCTAssertEqualObjects([DBTestProvider callProviderString:objcProviderStr], @"world");

    DJProvider<NSNumber *> *objcProviderInt = [DJProvider providerWithBlock:^NSNumber*() {
        return @123;
    }];
    XCTAssertEqual([DBTestProvider callProviderInt:objcProviderInt], 123);

    DBNestedProvider* np = [DBTestProvider getNestedProvider];
    DJProvider<NSNumber *> *providerFunc = np.s;
    XCTAssertEqual([[providerFunc get] intValue], 99);

    DJProvider<NSNumber *> *nestedProvider = [DJProvider providerWithBlock:^NSNumber*() {
        return @55;
    }];
    XCTAssertEqual([DBTestProvider callNestedProvider:[DBNestedProvider nestedProviderWithS:nestedProvider]], 55);

    DJProvider<DBSimpleObject *> *providerObj = [DBTestProvider getProviderObject];
    DBSimpleObject* obj = [providerObj get];
    XCTAssertEqual([obj getValue], 42);
    XCTAssertEqualObjects([obj getName], @"expensive");

    XCTAssertEqual([DBTestProvider callProviderObject:providerObj], 42);
    XCTAssertEqualObjects([DBTestProvider callProviderObjectGetName:providerObj], @"expensive");

    __block int creationCount = 0;
    DJProvider<DBSimpleObject *> *objcProviderObj = [DJProvider providerWithBlock:^DBSimpleObject*() {
        creationCount++;
        return [DBTestProvider createSimpleObject:999 name:@"objc-created"];
    }];

    XCTAssertEqual(creationCount, 0);
    XCTAssertEqual([DBTestProvider callProviderObject:objcProviderObj], 999);
    XCTAssertEqual(creationCount, 1);

    XCTAssertEqualObjects([DBTestProvider callProviderObjectGetName:objcProviderObj], @"objc-created");
    XCTAssertEqual(creationCount, 2);

    DBNestedProviderInterface* npi = [DBTestProvider getNestedProviderInterface];
    DJProvider<DBSimpleObject *> *objProvider = npi.obj;
    XCTAssertEqual([[objProvider get] getValue], 777);

    DJProvider<DBSimpleObject *> *nestedObjProvider = [DJProvider providerWithBlock:^DBSimpleObject*() {
        return [DBTestProvider createSimpleObject:888 name:@"nested-objc"];
    }];
    XCTAssertEqual([DBTestProvider callNestedProviderInterface:[DBNestedProviderInterface nestedProviderInterfaceWithObj:nestedObjProvider]], 888);

    __block int memoCreationCount = 0;
    id (^memoizedBlock)(void) = memoize(^DBSimpleObject*() {
        memoCreationCount++;
        return [DBTestProvider createSimpleObject:12345 name:@"memoized"];
    });
    DJProvider<DBSimpleObject *> *memoizedProvider = [DJProvider providerWithBlock:memoizedBlock];

    XCTAssertEqual(memoCreationCount, 0);
    XCTAssertEqual([DBTestProvider callProviderObject:memoizedProvider], 12345);
    XCTAssertEqual(memoCreationCount, 1);

    XCTAssertEqualObjects([DBTestProvider callProviderObjectGetName:memoizedProvider], @"memoized");
    XCTAssertEqual(memoCreationCount, 1);

    XCTAssertEqual([DBTestProvider callProviderObject:memoizedProvider], 12345);
    XCTAssertEqual(memoCreationCount, 1);

    DBSimpleObject* memoObj = [memoizedProvider get];
    XCTAssertEqual([memoObj getValue], 12345);
    XCTAssertEqualObjects([memoObj getName], @"memoized");
    XCTAssertEqual(memoCreationCount, 1);
}

@end

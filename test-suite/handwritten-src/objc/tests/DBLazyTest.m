#import <Foundation/Foundation.h>
#import <DBTestLazy.h>
#import <DBNestedLazy.h>
#import <DBNestedLazyInterface.h>
#import <DBSimpleObject.h>
#import <XCTest/XCTest.h>

@interface DBLazyTests : XCTestCase
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

@implementation DBLazyTests

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
    // Simple type tests
    // get lazy string from native and call it
    DJLazy<NSString *> *lazyStr = [DBTestLazy getLazyString];
    XCTAssertEqualObjects([lazyStr get], @"hello");

    // get lazy int from native and call it
    DJLazy<NSNumber *> *lazyInt = [DBTestLazy getLazyInt];
    XCTAssertEqual([[lazyInt get] intValue], 42);

    // create lazy string in objc and pass to native which calls it
    DJLazy<NSString *> *objcLazyStr = [DJLazy lazyWithBlock:^NSString*() {
        return @"world";
    }];
    XCTAssertEqualObjects([DBTestLazy callLazyString:objcLazyStr], @"world");

    // create lazy int in objc and pass to native which calls it
    DJLazy<NSNumber *> *objcLazyInt = [DJLazy lazyWithBlock:^NSNumber*() {
        return @123;
    }];
    XCTAssertEqual([DBTestLazy callLazyInt:objcLazyInt], 123);

    // test lazy as nested object
    DBNestedLazy* nl = [DBTestLazy getNestedLazy];
    DJLazy<NSNumber *> *lazyFunc = nl.l;
    XCTAssertEqual([[lazyFunc get] intValue], 99);

    DJLazy<NSNumber *> *nestedLazy = [DJLazy lazyWithBlock:^NSNumber*() {
        return @55;
    }];
    XCTAssertEqual([DBTestLazy callNestedLazy:[DBNestedLazy nestedLazyWithL:nestedLazy]], 55);

    // Interface lazy tests - main use case for deferred expensive object creation
    // get lazy object from native and call it
    DJLazy<DBSimpleObject *> *lazyObj = [DBTestLazy getLazyObject];
    DBSimpleObject* obj = [lazyObj get];
    XCTAssertEqual([obj getValue], 42);
    XCTAssertEqualObjects([obj getName], @"expensive");

    // Verify lazy object is called from native
    XCTAssertEqual([DBTestLazy callLazyObject:lazyObj], 42);
    XCTAssertEqualObjects([DBTestLazy callLazyObjectGetName:lazyObj], @"expensive");

    // Create lazy object in ObjC and pass to native
    // Track that lazy creation is deferred
    __block int creationCount = 0;
    DJLazy<DBSimpleObject *> *objcLazyObj = [DJLazy lazyWithBlock:^DBSimpleObject*() {
        creationCount++;
        return [DBTestLazy createSimpleObject:999 name:@"objc-created"];
    }];
    
    // Object not created yet
    XCTAssertEqual(creationCount, 0);
    
    // Call from native - object created on demand
    XCTAssertEqual([DBTestLazy callLazyObject:objcLazyObj], 999);
    XCTAssertEqual(creationCount, 1);
    
    XCTAssertEqualObjects([DBTestLazy callLazyObjectGetName:objcLazyObj], @"objc-created");
    XCTAssertEqual(creationCount, 2); // Created again

    // Test lazy interface as nested object
    DBNestedLazyInterface* nli = [DBTestLazy getNestedLazyInterface];
    DJLazy<DBSimpleObject *> *objLazy = nli.obj;
    XCTAssertEqual([[objLazy get] getValue], 777);

    DJLazy<DBSimpleObject *> *nestedObjLazy = [DJLazy lazyWithBlock:^DBSimpleObject*() {
        return [DBTestLazy createSimpleObject:888 name:@"nested-objc"];
    }];
    XCTAssertEqual([DBTestLazy callNestedLazyInterface:[DBNestedLazyInterface nestedLazyInterfaceWithObj:nestedObjLazy]], 888);

    // Test memoized lazy - verify factory is only called once
    __block int memoCreationCount = 0;
    id (^memoizedBlock)(void) = memoize(^DBSimpleObject*() {
        memoCreationCount++;
        return [DBTestLazy createSimpleObject:12345 name:@"memoized"];
    });
    DJLazy<DBSimpleObject *> *memoizedLazy = [DJLazy lazyWithBlock:memoizedBlock];

    // Not created yet
    XCTAssertEqual(memoCreationCount, 0);

    // First call to C++ - creates the object
    XCTAssertEqual([DBTestLazy callLazyObject:memoizedLazy], 12345);
    XCTAssertEqual(memoCreationCount, 1);

    // Second call to C++ - should NOT create a new object (memoized)
    XCTAssertEqualObjects([DBTestLazy callLazyObjectGetName:memoizedLazy], @"memoized");
    XCTAssertEqual(memoCreationCount, 1); // Still 1!

    // Third call to C++ - still memoized
    XCTAssertEqual([DBTestLazy callLazyObject:memoizedLazy], 12345);
    XCTAssertEqual(memoCreationCount, 1); // Still 1!

    // Also test calling get directly in ObjC
    DBSimpleObject* memoObj = [memoizedLazy get];
    XCTAssertEqual([memoObj getValue], 12345);
    XCTAssertEqualObjects([memoObj getName], @"memoized");
    XCTAssertEqual(memoCreationCount, 1); // Still 1!
}

@end

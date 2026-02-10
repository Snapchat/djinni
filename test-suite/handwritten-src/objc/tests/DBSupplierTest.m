#import <Foundation/Foundation.h>
#import <DBTestSupplier.h>
#import <DBNestedSupplier.h>
#import <DBNestedSupplierInterface.h>
#import <DBSimpleObject.h>
#import <XCTest/XCTest.h>

@interface DBSupplierTests : XCTestCase
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

@implementation DBSupplierTests

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
    // get supplier string from native and call it
    DJSupplier<NSString *> *supplierStr = [DBTestSupplier getSupplierString];
    XCTAssertEqualObjects([supplierStr get], @"hello");

    // get supplier int from native and call it
    DJSupplier<NSNumber *> *supplierInt = [DBTestSupplier getSupplierInt];
    XCTAssertEqual([[supplierInt get] intValue], 42);

    // create supplier string in objc and pass to native which calls it
    DJSupplier<NSString *> *objcSupplierStr = [DJSupplier supplierWithBlock:^NSString*() {
        return @"world";
    }];
    XCTAssertEqualObjects([DBTestSupplier callSupplierString:objcSupplierStr], @"world");

    // create supplier int in objc and pass to native which calls it
    DJSupplier<NSNumber *> *objcSupplierInt = [DJSupplier supplierWithBlock:^NSNumber*() {
        return @123;
    }];
    XCTAssertEqual([DBTestSupplier callSupplierInt:objcSupplierInt], 123);

    // test supplier as nested object
    DBNestedSupplier* ns = [DBTestSupplier getNestedSupplier];
    DJSupplier<NSNumber *> *supplierFunc = ns.s;
    XCTAssertEqual([[supplierFunc get] intValue], 99);

    DJSupplier<NSNumber *> *nestedSupplier = [DJSupplier supplierWithBlock:^NSNumber*() {
        return @55;
    }];
    XCTAssertEqual([DBTestSupplier callNestedSupplier:[DBNestedSupplier nestedSupplierWithS:nestedSupplier]], 55);

    // Interface supplier tests - main use case for deferred expensive object creation
    // get supplier object from native and call it
    DJSupplier<DBSimpleObject *> *supplierObj = [DBTestSupplier getSupplierObject];
    DBSimpleObject* obj = [supplierObj get];
    XCTAssertEqual([obj getValue], 42);
    XCTAssertEqualObjects([obj getName], @"expensive");

    // Verify supplier object is called from native
    XCTAssertEqual([DBTestSupplier callSupplierObject:supplierObj], 42);
    XCTAssertEqualObjects([DBTestSupplier callSupplierObjectGetName:supplierObj], @"expensive");

    // Create supplier object in ObjC and pass to native
    // Track that creation is deferred
    __block int creationCount = 0;
    DJSupplier<DBSimpleObject *> *objcSupplierObj = [DJSupplier supplierWithBlock:^DBSimpleObject*() {
        creationCount++;
        return [DBTestSupplier createSimpleObject:999 name:@"objc-created"];
    }];

    // Object not created yet
    XCTAssertEqual(creationCount, 0);

    // Call from native - object created on demand
    XCTAssertEqual([DBTestSupplier callSupplierObject:objcSupplierObj], 999);
    XCTAssertEqual(creationCount, 1);

    XCTAssertEqualObjects([DBTestSupplier callSupplierObjectGetName:objcSupplierObj], @"objc-created");
    XCTAssertEqual(creationCount, 2); // Created again

    // Test supplier interface as nested object
    DBNestedSupplierInterface* nsi = [DBTestSupplier getNestedSupplierInterface];
    DJSupplier<DBSimpleObject *> *objSupplier = nsi.obj;
    XCTAssertEqual([[objSupplier get] getValue], 777);

    DJSupplier<DBSimpleObject *> *nestedObjSupplier = [DJSupplier supplierWithBlock:^DBSimpleObject*() {
        return [DBTestSupplier createSimpleObject:888 name:@"nested-objc"];
    }];
    XCTAssertEqual([DBTestSupplier callNestedSupplierInterface:[DBNestedSupplierInterface nestedSupplierInterfaceWithObj:nestedObjSupplier]], 888);

    // Test memoized supplier - verify factory is only called once
    __block int memoCreationCount = 0;
    id (^memoizedBlock)(void) = memoize(^DBSimpleObject*() {
        memoCreationCount++;
        return [DBTestSupplier createSimpleObject:12345 name:@"memoized"];
    });
    DJSupplier<DBSimpleObject *> *memoizedSupplier = [DJSupplier supplierWithBlock:memoizedBlock];

    // Not created yet
    XCTAssertEqual(memoCreationCount, 0);

    // First call to C++ - creates the object
    XCTAssertEqual([DBTestSupplier callSupplierObject:memoizedSupplier], 12345);
    XCTAssertEqual(memoCreationCount, 1);

    // Second call to C++ - should NOT create a new object (memoized)
    XCTAssertEqualObjects([DBTestSupplier callSupplierObjectGetName:memoizedSupplier], @"memoized");
    XCTAssertEqual(memoCreationCount, 1); // Still 1!

    // Third call to C++ - still memoized
    XCTAssertEqual([DBTestSupplier callSupplierObject:memoizedSupplier], 12345);
    XCTAssertEqual(memoCreationCount, 1); // Still 1!

    // Also test calling get directly in ObjC
    DBSimpleObject* memoObj = [memoizedSupplier get];
    XCTAssertEqual([memoObj getValue], 12345);
    XCTAssertEqualObjects([memoObj getName], @"memoized");
    XCTAssertEqual(memoCreationCount, 1); // Still 1!
}

@end

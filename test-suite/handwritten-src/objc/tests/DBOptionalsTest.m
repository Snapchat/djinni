#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#import "DBOptionalInterface.h"
#import "DBOptionalTestHelpers.h"

@interface DBOptionalsTest : XCTestCase

@end

@implementation DBOptionalsTest

- (void)setUp
{
    [super setUp];
}

- (void)tearDown
{
    [super tearDown];
}

- (void)testMixedRecordTranslation
{
    DBMixedRecord * rec = [DBMixedRecord mixedRecordWithReqInt:(int32_t)5
                                                     reqRecord:[DBOptionalTestHelpers getBaseRecord]];
    rec.interfaceVal = [DBOptionalTestHelpers optionalInterface];

    DBMixedRecord * expectedRec = [DBOptionalTestHelpers getMixedRecord];

    XCTAssertEqualObjects(rec.interfaceVal, expectedRec.interfaceVal);
    XCTAssertEqual(rec.reqInt, expectedRec.reqInt);
    XCTAssertEqualObjects(rec.optRecord, expectedRec.optRecord);
    XCTAssertEqual(rec.reqRecord.val, expectedRec.reqRecord.val);
}

- (void)testOptionalRecordTranslation
{
    DBOptionalRecord * rec = [DBOptionalRecord optionalRecord];
    rec.optInt = [NSNumber numberWithInt:6];

    XCTAssertEqualObjects(rec.optInterface, [[DBOptionalTestHelpers getOptionalRecord] optInterface]);
    XCTAssertEqualObjects(rec.optRecord, [[DBOptionalTestHelpers getOptionalRecord] optRecord]);
    XCTAssertEqualObjects(rec.optInt, [[DBOptionalTestHelpers getOptionalRecord] optInt]);
}

- (void)testTranslation
{
    DBMixedRecord * rec = [DBMixedRecord mixedRecordWithReqInt:(int32_t)1
                                                     reqRecord:[DBOptionalTestHelpers getBaseRecord]];
    rec.optRecord = [DBBaseRecord baseRecordWithVal:(int32_t)5];

    DBMixedRecord * translatedRec = [DBOptionalTestHelpers mixedRecordId:rec];

    XCTAssertEqualObjects(rec.interfaceVal, translatedRec.interfaceVal);
    XCTAssertEqual(rec.reqInt, translatedRec.reqInt);
    XCTAssertEqual(rec.optRecord.val, translatedRec.optRecord.val);
    XCTAssertEqual(rec.reqRecord.val, translatedRec.reqRecord.val);
}

@end

#import <Foundation/Foundation.h>
#import <DBProtoTests.h>
#import <XCTest/XCTest.h>

@interface DBProtoTest : XCTestCase
@end

@implementation DBProtoTest

- (void)setUp
{
    [super setUp];
}

- (void)tearDown
{
    [super tearDown];
}

- (void)testObjcToNative
{
    DJTestAddressBook* proto = [DJTestAddressBook new];
    DJTestPerson* tom = [DJTestPerson new];
    tom.name = @"tom";
    tom.id_p = 1;
    DJTestPerson* jerry = [DJTestPerson new];
    jerry.name = @"jerry";
    jerry.id_p = 2;
    [proto.peopleArray addObject:tom];
    [proto.peopleArray addObject:jerry];
    NSArray<NSString *> * r = [DBProtoTests protoToStrings:proto];
    XCTAssertEqualObjects(r[0], @"tom");
    XCTAssertEqualObjects(r[1], @"jerry");
}

- (void) testNativeToObjc
{
    DJTestAddressBook* proto = [DBProtoTests stringsToProto:@[@"tom", @"jerry"]];
    XCTAssertEqualObjects(proto.peopleArray[0].name, @"tom");
    XCTAssertEqualObjects(proto.peopleArray[1].name, @"jerry");
}

- (void) testEmbeddedProto {
    DJTestPerson* p = [DJTestPerson new];
    p.name = @"tom";
    p.id_p = 1;
    DBRecordWithEmbeddedProto* rec = [DBRecordWithEmbeddedProto RecordWithEmbeddedProtoWithPerson:p];
    NSString* s = [DBProtoTests embeddedProtoToString:rec];
    XCTAssertEqualObjects(s, p.name);
    rec = [DBProtoTests stringToEmbeddedProto:s];
    XCTAssertEqualObjects(s, rec.person.name);
}

- (void) testCppProto {
    ::djinni::test2::PersistingState cppProto;
    cppProto.set_user_id("tom");
    NSString* s = [DBProtoTests cppProtoToString:cppProto];
    XCTAssertEqualObjects(s, @"tom");
    cppProto = [DBProtoTests stringToCppProto:@"tom"];
    XCTAssert(cppProto.user_id() == "tom");
}

- (void) testEmbeddedCppProto {
    ::djinni::test2::PersistingState state;
    state.set_user_id("tom");
    DBRecordWithEmbeddedCppProto* rec = [DBRecordWithEmbeddedCppProto RecordWithEmbeddedCppProtoWithState:state];
    NSString* s = [DBProtoTests embeddedCppProtoToString:rec];
    XCTAssertEqualObjects(s, @"tom");
    rec = [DBProtoTests stringToEmbeddedCppProto:@"tom"];
    XCTAssert(rec.state.user_id() == "tom");    
}

- (void) testBoxing {
    DJTestPerson* tom = [DJTestPerson new];
    tom.name = @"tom";
    tom.id_p = 1;
    DJTestPerson* jerry = [DJTestPerson new];
    jerry.name = @"jerry";
    jerry.id_p = 2;
    NSArray<DJTestPerson*>* plist = @[tom, jerry];
    NSArray<NSString*>* slist = [DBProtoTests protoListToStrings:plist];
    NSArray<NSString*>* names = @[@"tom", @"jerry"];
    XCTAssertEqualObjects(slist, names);

    NSArray<DJTestPerson*>* plist2 = [DBProtoTests stringsToProtoList:slist];
    XCTAssertEqualObjects(plist2, plist);

    NSString* s = [DBProtoTests optionalProtoToString:tom];
    XCTAssertEqualObjects(s, @"tom");

    DJTestPerson* p = [DBProtoTests stringToOptionalProto:@"tom"];
    XCTAssertEqualObjects(p, tom);

    DJOutcome<DJTestPerson*, NSNumber*>* r = [DBProtoTests stringToProtoOutcome:@"tom"];
    XCTAssertEqualObjects(r, [DJOutcome fromResult:tom]);
}

@end

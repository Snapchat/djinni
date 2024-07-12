import XCTest
@testable import ProtobufTest
@testable import TestSuite
@testable import TestSuiteBridge

typealias Person = Djinni_Test_Person
typealias AddressBook = Djinni_Test_AddressBook

final class ProtoTest: XCTestCase {
    func testSwiftToNative() throws {
        let p1 = Person.with {$0.name = "tom"; $0.id = 1}
        let p2 = Person.with {$0.name = "jerry"; $0.id = 2}
        let proto = AddressBook.with {$0.people = [p1, p2]}
        let r = try ProtoTests_statics.protoToStrings(proto)
        XCTAssertEqual(r[0], "tom")
        XCTAssertEqual(r[1], "jerry")
    }

    func testNativeToSwift() throws {
        let names = ["tom", "jerry"]
        let proto = try ProtoTests_statics.stringsToProto(names)
        XCTAssertEqual(proto.people[0].name, "tom")
        XCTAssertEqual(proto.people[1].name, "jerry")
    }

    func testZeroSizeNativeToSwift() throws {
        let proto = try ProtoTests_statics.stringsToProto([])
        let serialized: Data = try proto.serializedBytes()
        XCTAssertEqual(serialized.count, 0)
    }

    func testEmbeddedProto() throws {
        let p = Person.with {$0.name = "tom"; $0.id = 1}
        let rec = RecordWithEmbeddedProto(person:p)
        let s = try ProtoTests_statics.embeddedProtoToString(rec)
        XCTAssertEqual(s, p.name)
        let rec2 = try ProtoTests_statics.stringToEmbeddedProto(s);
        XCTAssertEqual(s, rec2.person.name)
    }

    func testBoxing() throws {
        let p1 = Person.with {$0.name = "tom"; $0.id = 1}
        let p2 = Person.with {$0.name = "jerry"; $0.id = 2}
        var plist = [p1, p2]
        let slist = try ProtoTests_statics.protoListToStrings(plist)
        XCTAssertEqual(slist[0], "tom")
        XCTAssertEqual(slist[1], "jerry")

        plist = try ProtoTests_statics.stringsToProtoList(slist)
        XCTAssertEqual(plist[0].name, "tom")
        XCTAssertEqual(plist[1].name, "jerry")

        let s = try ProtoTests_statics.optionalProtoToString(p1)
        XCTAssertEqual(s, "tom")
        let p = try ProtoTests_statics.stringToOptionalProto("tom")
        XCTAssertEqual(p?.name, "tom")

        let r = try ProtoTests_statics.stringToProtoOutcome("tom")
        XCTAssertEqual(try r.get().name, "tom")
    }
}

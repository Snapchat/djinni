import XCTest
@testable import TestSuite
@testable import TestSuiteBridge

final class SetRecordTest: XCTestCase {

    func testCppSetToSwiftSet() throws {
        let sSetRecord = try TestHelpers_statics.getSetRecord()
        let sSet = sSetRecord.set
        XCTAssertEqual(3, sSet.count)
        XCTAssertTrue(sSet.contains("StringA"))
        XCTAssertTrue(sSet.contains("StringB"))
        XCTAssertTrue(sSet.contains("StringC"))
    }

    func testSwiftSetToCppSet() throws {
        let sSetRecord = SetRecord(set: Set(["StringA", "StringB", "StringC"]), iset:Set([]))
        XCTAssertTrue(try TestHelpers_statics.checkSetRecord(sSetRecord))
    }
}

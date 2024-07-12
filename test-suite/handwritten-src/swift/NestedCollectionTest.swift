import XCTest
@testable import TestSuite
@testable import TestSuiteBridge

final class NestedCollectionTest: XCTestCase {
    let swiftNestedCollection = NestedCollection(setList: [Set(["String1", "String2"]), Set(["StringA", "StringB"])])
    
    func testCppNestedRecordToSwiftNestedCollection() throws {
        let converted = try TestHelpers_statics.getNestedCollection()
        XCTAssertEqual(swiftNestedCollection.setList, converted.setList)
    }

    func testSwiftNestedRecordToCppNestedCollection() throws {
        XCTAssertTrue(try TestHelpers_statics.checkNestedCollection(swiftNestedCollection))
    }
}

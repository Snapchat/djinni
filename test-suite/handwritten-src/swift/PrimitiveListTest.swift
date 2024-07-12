import XCTest
@testable import TestSuite
@testable import TestSuiteBridge

final class PrimitivesListTest: XCTestCase {

    let swiftPrimitiveList: PrimitiveList = PrimitiveList(list: [1, 2, 3])
    
    func testSwiftPrimitiveListToCpp() throws {
        XCTAssertTrue(try TestHelpers_statics.checkPrimitiveList(swiftPrimitiveList))
    }
    func testCppPrimitiveListToSwift() throws {
        let converted = try TestHelpers_statics.getPrimitiveList()
        XCTAssertEqual(converted.list, swiftPrimitiveList.list)
    }
    func testBinary() throws {
        let b = Data([1,2,3])
        XCTAssertEqual(try TestHelpers_statics.idBinary(b), b)
    }
}

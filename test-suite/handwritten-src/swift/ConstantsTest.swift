import XCTest
import DjinniSupport
@testable import TestSuite

final class ConstantsTest: XCTestCase {
    func testConstPrimitives() throws {
        XCTAssertEqual(Constants.i8Constant, 1)
        XCTAssertEqual(Constants.optI8Constant, 1)
        XCTAssertEqual(Constants.stringConstant, "string-constant")
    }

    func testConstRecords() throws {
        XCTAssertEqual(Constants.objectConstant, ConstantRecord(someInteger:3, someString:"string-constant"))
    }
}

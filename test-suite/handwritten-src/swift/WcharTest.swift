import XCTest
@testable import TestSuite

final class WcharTest: XCTestCase {
    let STR1: String = "some string with unicode \u{0000}, \u{263A}, 💩 symbols";
    let STR2: String = "another string with unicode \u{263B}, 💨 symbols";

    func test() throws {
        XCTAssertEqual(try WcharTestHelpers_statics.getRecord().s, STR1)
        XCTAssertEqual(try WcharTestHelpers_statics.getString(), STR2)
        XCTAssertTrue(try WcharTestHelpers_statics.checkString(STR2))
        XCTAssertTrue(try WcharTestHelpers_statics.checkRecord(WcharTestRec(s:STR1)))
    }
}

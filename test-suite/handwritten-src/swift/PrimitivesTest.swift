import XCTest
@testable import TestSuite
@testable import TestSuiteBridge

final class PrimitivesTest: XCTestCase {
    func testPrimitives() throws {
        let p = AssortedPrimitives(b:true, eight:123, sixteen:20000, thirtytwo:1000000000, sixtyfour:1234567890123456789, fthirtytwo:1.23, fsixtyfour:1.23,
                                   oB:true, oEight:123, oSixteen:20000, oThirtytwo:1000000000, oSixtyfour:1234567890123456789, oFthirtytwo:1.23, oFsixtyfour:1.23)
        XCTAssertEqual(p, try TestHelpers_statics.assortedPrimitivesId(p))
    }
}

import XCTest
@testable import TestSuite
@testable import TestSuiteBridge

final class EnumTest: XCTestCase {

    func testEnumToString() {
        XCTAssertEqual(String(describing: Color.red), "red")
        XCTAssertEqual(String(describing: Color.orange), "orange")
        XCTAssertEqual(String(describing: Color.yellow), "yellow")
        XCTAssertEqual(String(describing: Color.green), "green")
        XCTAssertEqual(String(describing: Color.blue), "blue")
        XCTAssertEqual(String(describing: Color.indigo), "indigo")
        XCTAssertEqual(String(describing: Color.violet), "violet")
    }

    func testEnumKey() throws {
        var m = Dictionary<Color, String>()
        m[Color.red] = "red"
        m[Color.orange] = "orange"
        m[Color.yellow] = "yellow"
        m[Color.green] = "green"
        m[Color.blue] = "blue"
        m[Color.indigo] = "indigo"
        m[Color.violet] = "violet"
        try TestHelpers_statics.checkEnumMap(m)
    }

    func testAccessFlagRoundtrip() throws {
        let flags: [AccessFlags] = [.nobody, .everybody, .ownerRead, [.ownerRead, .ownerWrite], [.ownerRead, .ownerWrite, .ownerExecute]]
        for flag in flags {
            XCTAssertEqual(flag, try FlagRoundtrip_statics.roundtripAccess(flag))
            XCTAssertEqual(flag, try FlagRoundtrip_statics.roundtripAccessBoxed(flag))
        }
        XCTAssertEqual(nil, try FlagRoundtrip_statics.roundtripAccessBoxed(nil))
    }

    func testEmptyFlagRoundtrip() throws {
        let flags: [EmptyFlags] = [.none, .all]
        for flag in flags {
            XCTAssertEqual(flag, try FlagRoundtrip_statics.roundtripEmpty(flag))
            XCTAssertEqual(flag, try FlagRoundtrip_statics.roundtripEmptyBoxed(flag))
        }
        XCTAssertEqual(nil, try FlagRoundtrip_statics.roundtripEmptyBoxed(nil))
    }
}

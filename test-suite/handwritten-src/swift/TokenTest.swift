import XCTest
@testable import TestSuite
@testable import TestSuiteBridge

final class TokenTest: XCTestCase {

    class SwiftToken: UserToken {
        func whoami() throws -> String { return "Swift" }
    }

    func testTokens() throws {
        let st = SwiftToken()
        XCTAssertIdentical(try TestHelpers_statics.tokenId(t:st), st)
    }

    func testNullToken() throws {
        XCTAssertNil(try TestHelpers_statics.tokenId(t:nil))
    }

    func testCppToken() throws {
        let ct = try TestHelpers_statics.createCppToken();
        XCTAssertIdentical(try TestHelpers_statics.tokenId(t:ct), ct)
        try TestHelpers_statics.checkCppToken(t:ct)
    }

    func testTokenType() throws {
        try TestHelpers_statics.checkTokenType(t:SwiftToken(), type:"Swift")
        try TestHelpers_statics.checkTokenType(t:TestHelpers_statics.createCppToken(), type:"C++");
        var threw = false
        do {
            try TestHelpers_statics.checkTokenType(t: SwiftToken(), type:"foo")
        } catch {
            threw = true
        }
        XCTAssertTrue(threw)
    }

    func testNotCppToken() {
        var threw = false
        do {
            try TestHelpers_statics.checkCppToken(t:SwiftToken())
        } catch {
            threw = true
        }
        XCTAssertTrue(threw)
    }
}

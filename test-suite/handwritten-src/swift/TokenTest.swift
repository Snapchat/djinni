import XCTest
@testable import TestSuite

final class TokenTest: XCTestCase {

    class SwiftToken: UserToken {
        func whoami() throws -> String { return "Swift" }
    }

    func testTokens() throws {
        let st = SwiftToken()
        XCTAssertIdentical(try TestHelpers_statics.tokenId(st), st)
    }

    func testNullToken() throws {
        XCTAssertNil(try TestHelpers_statics.tokenId(nil))
    }

    func testCppToken() throws {
        let ct = try TestHelpers_statics.createCppToken();
        XCTAssertIdentical(try TestHelpers_statics.tokenId(ct), ct)
        try TestHelpers_statics.checkCppToken(ct)
    }

    func testTokenType() throws {
        try TestHelpers_statics.checkTokenType(SwiftToken(), type:"Swift")
        try TestHelpers_statics.checkTokenType(TestHelpers_statics.createCppToken(), type:"C++");
        var threw = false
        do {
            try TestHelpers_statics.checkTokenType(SwiftToken(), type:"foo")
        } catch {
            threw = true
        }
        XCTAssertTrue(threw)
    }

    func testNotCppToken() {
        var threw = false
        do {
            try TestHelpers_statics.checkCppToken(SwiftToken())
        } catch {
            threw = true
        }
        XCTAssertTrue(threw)
    }
}

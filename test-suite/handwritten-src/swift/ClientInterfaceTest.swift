import XCTest
@testable import TestSuite
@testable import TestSuiteBridge

struct ClientInterfaceError: Error {
    var msg: String
}

class ClientInterfaceImpl: ClientInterface {
    func getRecord(recordId: Int64, utf8string: String, misc: Optional<String>) throws -> TestSuite.ClientReturnedRecord {
        if (utf8string != "Non-ASCII /\0 非 ASCII 字符" && utf8string != "Hello World!") {
            throw ClientInterfaceError(msg: "Unexpected string. Check UTF-8?")
        }
        return ClientReturnedRecord(recordId:recordId, content:utf8string, misc:misc)
    }
    func identifierCheck(data: Data, r: Int32, jret: Int64) throws -> Double {
        return 0.0
    }
    func returnStr() throws -> String {
        return "test"
    }
    func methTakingInterface(i: TestSuite.ClientInterface) throws -> String {
        return try i.returnStr()
    }
    func methTakingOptionalInterface(i: Optional<TestSuite.ClientInterface>) throws -> String {
        if (i != nil) {
            return try i!.returnStr()
        } else {
            return ""
        }
    }
}

final class ClientInterfaceTest: XCTestCase {
    var sClientInterface: ClientInterface? = nil

    override func setUp() {
        sClientInterface = ClientInterfaceImpl()
    }

    func testClientReturn() throws {
        try TestHelpers_statics.checkClientInterfaceAscii(i: sClientInterface!)
    }

    func testClientReturnUTF8() throws {
        try TestHelpers_statics.checkClientInterfaceNonascii(i: sClientInterface!)
    }

    func testClientInterfaceArgs() throws {
        try TestHelpers_statics.checkClientInterfaceArgs(i: sClientInterface!)
    }

    func testReverseClientInterfaceArgs() throws {
        let i = try ReverseClientInterface_statics.create()
        XCTAssertEqual(try i.methTakingInterface(i:i), "test")
        XCTAssertEqual(try i.methTakingOptionalInterface(i:i), "test")
    }
}

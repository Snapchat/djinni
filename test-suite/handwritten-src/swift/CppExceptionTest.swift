import XCTest
import DjinniSupport
@testable import TestSuite

final class CppExceptionTest: XCTestCase {

    class SwiftThrower: ThrowingInterface {
        func throwException() throws -> Void {
            throw DjinniError("Swift Error")
        }
    }

    let cppInterface = (try? CppException_statics.get())!
    
    func testCppException() throws {
        var thrown: DjinniError? = nil
        do {
            let _ = try cppInterface.throwAnException()
        } catch let e as DjinniError {
            thrown = e
        }
        XCTAssertEqual(try XCTUnwrap(thrown).errorMessage, "Exception Thrown")
    }

    func testSwiftExceptionHandledByCpp() throws {
        let res = try cppInterface.callThrowingAndCatch(SwiftThrower())
        XCTAssertEqual(res, "Swift Error")
    }
}

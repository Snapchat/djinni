import XCTest
@testable import TestSuite
@testable import TestSuiteBridge

final class OutcomeTest: XCTestCase {
    func test() throws {
        // construct result outcome in native and pass to swift
        let r = try TestOutcome_statics.getSuccessOutcome()
        XCTAssertEqual(r, Result.success("hello"))

        // construct error outcome in native and pass to swift
        let e = try TestOutcome_statics.getErrorOutcome()
        XCTAssertEqual(e, Result.failure(42))

        // result outcome and error outcome does not compare equal
        XCTAssertNotEqual(r, e)

        // construct result outcome in swift then pass to native and back
        XCTAssertEqual(try TestOutcome_statics.putSuccessOutcome(x:.success("hello")), "hello");
        // construct error outcome in swift then pass to native and back
        XCTAssertEqual(try TestOutcome_statics.putErrorOutcome(x:.failure(42)), 42);

        // Hash equal
        XCTAssertEqual(r.hashValue, Result<String, Int32>.success("hello").hashValue)
        XCTAssertEqual(e.hashValue, Result<String, Int32>.failure(42).hashValue)
        // Hash not equal between result and error
        XCTAssertNotEqual(r.hashValue, Result<Int32, String>.failure("hello").hashValue)
        XCTAssertNotEqual(e.hashValue, Result<Int32, String>.success(42).hashValue)

        // test outcome as nested object
        let nr = try TestOutcome_statics.getNestedSuccessOutcome()
        XCTAssertEqual(nr.o, .success(42))
        XCTAssertEqual(nr, NestedOutcome(o:.success(42)))

        let ne = try TestOutcome_statics.getNestedErrorOutcome()
        XCTAssertEqual(ne.o, .failure("hello"))
        XCTAssertEqual(ne, NestedOutcome(o:.failure("hello")))

        XCTAssertEqual(try TestOutcome_statics.putNestedSuccessOutcome(x:NestedOutcome(o:.success(42))), 42)
        XCTAssertEqual(try TestOutcome_statics.putNestedErrorOutcome(x:NestedOutcome(o:.failure("hello"))), "hello")
    }
}

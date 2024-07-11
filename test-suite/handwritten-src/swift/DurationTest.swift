import XCTest
@testable import TestSuite
@testable import TestSuiteBridge

extension Duration {
    static func hours(_ h: Double) -> Duration {return .seconds(h*3600)}
    static func minutes(_ h: Double) -> Duration {return .seconds(h*60)}
}

final class DurationTest: XCTestCase {
    func test() throws {
        XCTAssertEqual(try TestDuration_statics.hoursString(dt:.hours(1)), "1")
        XCTAssertEqual(try TestDuration_statics.minutesString(dt:.minutes(1)), "1")
        XCTAssertEqual(try TestDuration_statics.secondsString(dt:.seconds(1)), "1")
        XCTAssertEqual(try TestDuration_statics.millisString(dt:.milliseconds(1)), "1")
        XCTAssertEqual(try TestDuration_statics.microsString(dt:.microseconds(1)), "1")
        XCTAssertEqual(try TestDuration_statics.nanosString(dt:.nanoseconds(1)), "1")

        XCTAssertEqual(try TestDuration_statics.hours(count:1), .hours(1))
        XCTAssertEqual(try TestDuration_statics.minutes(count:1), .minutes(1))
        XCTAssertEqual(try TestDuration_statics.seconds(count:1), .seconds(1))
        XCTAssertEqual(try TestDuration_statics.millis(count:1), .milliseconds(1))
        XCTAssertEqual(try TestDuration_statics.micros(count:1), .microseconds(1))
        XCTAssertEqual(try TestDuration_statics.nanos(count:1), .nanoseconds(1))

        XCTAssertEqual(try TestDuration_statics.hoursf(count:1.5), .minutes(90))
        XCTAssertEqual(try TestDuration_statics.minutesf(count:1.5), .seconds(90))
        XCTAssertEqual(try TestDuration_statics.secondsf(count:1.5), .milliseconds(1500))
        XCTAssertEqual(try TestDuration_statics.millisf(count:1.5), .microseconds(1500))
        XCTAssertEqual(try TestDuration_statics.microsf(count:1.5), .nanoseconds(1500))
        XCTAssertEqual(try TestDuration_statics.nanosf(count:1.0), .nanoseconds(1))

        XCTAssertEqual(try TestDuration_statics.box(count:1), .seconds(1))
        XCTAssertNil(try TestDuration_statics.box(count:-1))

        XCTAssertEqual(try TestDuration_statics.unbox(dt:.seconds(1)), 1)
        XCTAssertEqual(try TestDuration_statics.unbox(dt:nil), -1)
    }
}

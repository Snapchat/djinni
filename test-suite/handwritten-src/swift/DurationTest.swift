import XCTest
@testable import TestSuite

extension Duration {
    static func hours(_ h: Double) -> Duration {return .seconds(h*3600)}
    static func minutes(_ h: Double) -> Duration {return .seconds(h*60)}
}

final class DurationTest: XCTestCase {
    func test() throws {
        XCTAssertEqual(try TestDuration_statics.hoursString(.hours(1)), "1")
        XCTAssertEqual(try TestDuration_statics.minutesString(.minutes(1)), "1")
        XCTAssertEqual(try TestDuration_statics.secondsString(.seconds(1)), "1")
        XCTAssertEqual(try TestDuration_statics.millisString(.milliseconds(1)), "1")
        XCTAssertEqual(try TestDuration_statics.microsString(.microseconds(1)), "1")
        XCTAssertEqual(try TestDuration_statics.nanosString(.nanoseconds(1)), "1")

        XCTAssertEqual(try TestDuration_statics.hours(1), .hours(1))
        XCTAssertEqual(try TestDuration_statics.minutes(1), .minutes(1))
        XCTAssertEqual(try TestDuration_statics.seconds(1), .seconds(1))
        XCTAssertEqual(try TestDuration_statics.millis(1), .milliseconds(1))
        XCTAssertEqual(try TestDuration_statics.micros(1), .microseconds(1))
        XCTAssertEqual(try TestDuration_statics.nanos(1), .nanoseconds(1))

        XCTAssertEqual(try TestDuration_statics.hoursf(1.5), .minutes(90))
        XCTAssertEqual(try TestDuration_statics.minutesf(1.5), .seconds(90))
        XCTAssertEqual(try TestDuration_statics.secondsf(1.5), .milliseconds(1500))
        XCTAssertEqual(try TestDuration_statics.millisf(1.5), .microseconds(1500))
        XCTAssertEqual(try TestDuration_statics.microsf(1.5), .nanoseconds(1500))
        XCTAssertEqual(try TestDuration_statics.nanosf(1.0), .nanoseconds(1))

        XCTAssertEqual(try TestDuration_statics.box(1), .seconds(1))
        XCTAssertNil(try TestDuration_statics.box(-1))

        XCTAssertEqual(try TestDuration_statics.unbox(.seconds(1)), 1)
        XCTAssertEqual(try TestDuration_statics.unbox(nil), -1)
    }
}

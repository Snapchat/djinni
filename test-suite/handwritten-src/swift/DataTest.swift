import XCTest
import Foundation
import DjinniSupport
@testable import TestSuite

final class DataTest: XCTestCase {

    let test = (try? DataRefTest_statics.create())!

    func testSend() throws {
        let input: [UInt8] = [0, 1, 2, 3]
        try test.sendData(DJDataRef(Data(input)))
        let output = try test.retriveAsBin()
        XCTAssertEqual(Data(input), output)
    }

    // C++ shares the buffer rather than copying it, so its in-place change shows through.
    func testSendMutable() throws {
        let input: [UInt8] = [0, 1, 2, 3]
        let buf = DJDataRef(Data(input))
        try test.sendMutableData(buf)
        XCTAssertEqual(Data([3,2,1,0]), buf.data)
    }

    func testGenerateData() throws {
        let buf = try test.generateData()
        XCTAssertEqual(Data([0, 1, 2, 3]), buf.data)
    }

    func testDataFromVec() throws {
        let buf = try test.dataFromVec()
        XCTAssertEqual(Data([0, 1, 2, 3]), buf.data)
    }

    func testDataFromStr() throws {
        let buf = try test.dataFromStr()
        XCTAssertEqual("abcd".data(using:.utf8)![0...3], buf.data)
    }

    func testDataRefRoundTrip() throws {
        let buf = try test.generateData()
        XCTAssertEqual(buf, DJDataRef(Data([0, 1, 2, 3])))
        XCTAssertEqual(buf.hashValue, DJDataRef(Data([0, 1, 2, 3])).hashValue)
    }

    func testSendDataView() throws {
        let input: [UInt8] = [0, 1, 2, 3]
        let output = try input.withUnsafeBytes { try test.sendDataView($0) }
        XCTAssertEqual(Data(input), output)
    }

    func testRecvDataView() throws {
        let buf = try test.recvDataView()
        XCTAssertEqual(Data([0, 1, 2, 3]), Data(buf))
    }
}

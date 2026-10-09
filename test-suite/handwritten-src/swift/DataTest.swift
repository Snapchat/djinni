import XCTest
import Foundation
@testable import TestSuite

final class DataTest: XCTestCase {

    let test = (try? DataRefTest_statics.create())!

    func testSend() throws {
        let input: [UInt8] = [0, 1, 2, 3]
        let buf = Data(input)
        try test.sendData(buf as NSData)
        let output = try test.retriveAsBin()
        XCTAssertEqual(Data(input), output)
    }

    func testSendMutable() throws {
        let input: [UInt8] = [0, 1, 2, 3]
        let buf = NSMutableData()
        buf.append(Data(input))
        try test.sendMutableData(buf)
        XCTAssertEqual(Data([3,2,1,0]), buf as Data)
    }

    func testGenerateData() throws {
        let buf = try test.generateData()
        let output = buf as Data
        XCTAssertEqual(Data([0, 1, 2, 3]), output)
    }

    func testDataFromVec() throws {
        let buf = try test.dataFromVec()
        let output = buf as Data
        XCTAssertEqual(Data([0, 1, 2, 3]), output)
    }

    func testDataFromStr() throws {
        let buf = try test.dataFromStr()
        let output = buf as Data
        XCTAssertEqual("abcd".data(using:.utf8)![0...3], output)
    }
    
    func testSendDataView() throws {
        let input: [UInt8] = [0, 1, 2, 3]
        let buf = Data(input)
        let output = try test.sendDataView(buf as NSData)
        XCTAssertEqual(buf, output)
    }

    func testRecvDataView() throws {
        let buf = try test.recvDataView() as Data
        XCTAssertEqual(Data([0, 1, 2, 3]), buf)
    }
}

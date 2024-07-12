import XCTest
@testable import TestSuite
@testable import TestSuiteBridge

final class ArrayTest: XCTestCase {
    func testArrays() throws {
        let inputStrings = ["1", "2", "3"]
        let expectedStrings = ["3", "2", "1"]
        XCTAssertEqual(try TestArray_statics.testStringArray(inputStrings), expectedStrings)

        let inputIntegers: [Int32] = [1, 2, 3]
        let expectedIntegers: [Int32] = [3, 2, 1]
        XCTAssertEqual(try TestArray_statics.testIntArray(inputIntegers), expectedIntegers)

        let inputRecords = [Vec2(x:1, y:1), Vec2(x:2, y:2), Vec2(x:3, y:3)]
        let expectedRecords = [Vec2(x:3, y:3), Vec2(x:2, y:2), Vec2(x:1, y:1)]
        XCTAssertEqual(try TestArray_statics.testRecordArray(inputRecords), expectedRecords)

        let inputArrays: [[Int32]] = [[1,1,1], [2,2,2], [3,3,3]]
        let expectedArrays: [[Int32]] = [[3,3,3], [2,2,2], [1,1,1]]
        XCTAssertEqual(try TestArray_statics.testArrayOfArray(inputArrays), expectedArrays)
    }
}

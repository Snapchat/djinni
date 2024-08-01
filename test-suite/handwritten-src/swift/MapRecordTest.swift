import XCTest
@testable import TestSuite

final class MapRecordTest: XCTestCase {
    func testCppMapToSwiftMap() throws {
        checkSwiftMap(try TestHelpers_statics.getMap())
    }

    func testEmptyCppMapToSwiftMap() throws {
        XCTAssertEqual(0, try TestHelpers_statics.getEmptyMap().count)
    }

    func testCppMapListToSwiftMapList() throws {
        let sMapListRecord = try TestHelpers_statics.getMapListRecord()
        let sMapList = sMapListRecord.mapList
        XCTAssertEqual(1, sMapList.count)
        checkSwiftMap(sMapList[0])
    }

    func testSwiftMapToCppMap() throws {
        XCTAssertTrue(try TestHelpers_statics.checkMap(["String1": 1, "String2": 2, "String3": 3]))
    }

    func testEmptySwiftMapToCppMap() throws {
        XCTAssertTrue(try TestHelpers_statics.checkEmptyMap([:]))
    }

    func testSwiftMapListToCppMapList() throws {
        let sMapList: [Dictionary<String, Int64>] = [["String1": 1, "String2": 2, "String3": 3]]
        XCTAssertTrue(try TestHelpers_statics.checkMapListRecord(MapListRecord(mapList:sMapList)))
    }

    func checkSwiftMap(_ smap: Dictionary<String, Int64>) {
        XCTAssertEqual(3, smap.count)
        XCTAssertEqual(smap["String1"], 1)
        XCTAssertEqual(smap["String2"], 2)
        XCTAssertEqual(smap["String3"], 3)
    }
}

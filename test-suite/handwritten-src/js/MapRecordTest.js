class MapRecordTest {
    constructor(module) {
        this.m = module;
    }

    testCppMapToJavaMap() {
        this.checkJsMap(this.m.testsuite.TestHelpers.getMap());
    }

    testEmptyCppMapToJsMap() {
        assertEq(0, this.m.testsuite.TestHelpers.getEmptyMap().size);
    }

    testCppMapListToJsMapList() {
        var jsMapListRecord = this.m.testsuite.TestHelpers.getMapListRecord();
        var jsMapList = jsMapListRecord.mapList;
        assertEq(1, jsMapList.length);
        this.checkJsMap(jsMapList[0]);
    }

    testJsMapToCppMap() {
        assertTrue(this.m.testsuite.TestHelpers.checkMap(this.getJsMap()));
    }

    testEmptyJsMapToCppMap() {
        assertTrue(this.m.testsuite.TestHelpers.checkEmptyMap(new Map()));
    }

    testJsMapListToCppMapList() {
        var jsMapList = [];
        jsMapList.push(this.getJsMap());
        assertTrue(this.m.testsuite.TestHelpers.checkMapListRecord({mapList: jsMapList}));
    }

    getJsMap() {
        var jsMap = new Map();
        jsMap.set("String1", BigInt(1));
        jsMap.set("String2", BigInt(2));
        jsMap.set("String3", BigInt(3));
        return jsMap;
    }

    checkJsMap(jsMap) {
        assertEq(3, jsMap.size);
        assertEq(jsMap.get("String1"), BigInt(1));
        assertEq(jsMap.get("String2"), BigInt(2));
        assertEq(jsMap.get("String3"), BigInt(3));
    }
}

allTests.push(MapRecordTest);

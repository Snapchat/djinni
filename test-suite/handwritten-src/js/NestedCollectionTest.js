class NestedCollectionTest {
    constructor(module) {
        this.m = module;
    }
    setUp() {
        var jsSet1 = new Set();
        jsSet1.add("String1");
        jsSet1.add("String2");
        var jsSet2 = new Set();
        jsSet2.add("StringA");
        jsSet2.add("StringB");
        var jsList = [];
        jsList.push(jsSet1);
        jsList.push(jsSet2);
        this.jsNestedCollection = { setList: jsList };
    }
    testCppNestedRecordToJsNestedCollection() {
        var converted = this.m.testsuite.TestHelpers.getNestedCollection();
        assertEq(this.jsNestedCollection.setList, converted.setList);
    }

    testJsNestedRecordToCppNestedCollection() {
        assertTrue(this.m.testsuite.TestHelpers.checkNestedCollection(this.jsNestedCollection));
    }
}

allTests.push(NestedCollectionTest);

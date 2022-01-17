class SetRecordTest {
    constructor(module) {
        this.m = module;
    }
    testCppSetToJavaSet() {
        var jsSetRecord = this.m.testsuite.TestHelpers.getSetRecord();
        var jsSet = jsSetRecord.set;
        assertEq(3, jsSet.size);
        assertTrue(jsSet.has("StringA"));
        assertTrue(jsSet.has("StringB"));
        assertTrue(jsSet.has("StringC"));
    }

    testJavaSetToCppSet() {
        var jsSet = new Set();
        var iSet = new Set();
        jsSet.add("StringA");
        jsSet.add("StringB");
        jsSet.add("StringC");
        var jsSetRecord = {set: jsSet, iset: iSet};
        assertTrue(this.m.testsuite.TestHelpers.checkSetRecord(jsSetRecord));
    }
}
allTests.push(SetRecordTest);

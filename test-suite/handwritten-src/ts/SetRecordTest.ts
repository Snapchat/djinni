import {TestCase, allTests, assertEq, assertTrue} from "./testutils"
import * as test from "../../generated-src/ts/test";

class SetRecordTest extends TestCase {
    m: test.Test_statics;

    constructor(module: any) {
        super(module);
        this.m = <test.Test_statics>module;
    }
    testCppSetToJavaSet() {
        var jsSetRecord = this.m.TestHelpers.getSetRecord();
        var jsSet = jsSetRecord.set;
        assertEq(3, jsSet.size);
        assertTrue(jsSet.has("StringA"));
        assertTrue(jsSet.has("StringB"));
        assertTrue(jsSet.has("StringC"));
    }

    testJavaSetToCppSet() {
        let jsSet: Set<string> = new Set();
        let iSet: Set<number> = new Set();
        jsSet.add("StringA");
        jsSet.add("StringB");
        jsSet.add("StringC");
        let jsSetRecord: test.SetRecord = {set: jsSet, iset: iSet};
        assertTrue(this.m.TestHelpers.checkSetRecord(jsSetRecord));
    }
}
allTests.push(SetRecordTest);

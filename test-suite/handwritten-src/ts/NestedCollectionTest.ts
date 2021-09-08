import {TestCase, allTests, assertEq, assertTrue} from "./testutils"
import * as test from "../../generated-src/ts/test";

class NestedCollectionTest extends TestCase {
    m: test.Module_statics;
    jsNestedCollection: test.NestedCollection;

    constructor(module: any) {
        super(module);
        this.m = <test.Module_statics>module;
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
        var converted = this.m.TestHelpers.getNestedCollection();
        assertEq(this.jsNestedCollection.setList, converted.setList);
    }

    testJsNestedRecordToCppNestedCollection() {
        assertTrue(this.m.TestHelpers.checkNestedCollection(this.jsNestedCollection));
    }
}

allTests.push(NestedCollectionTest);

import {TestCase, allTests, assertArrayEq, assertTrue} from "./testutils"
import * as test from "../../generated-src/ts/test";

class PrimitiveListTest extends TestCase {
    m: test.Test_statics;
    jsPrimitiveList: test.PrimitiveList;
    constructor(module: test.Test_module_statics) {
        super(module);
        this.m = module.testsuite;
    }
    setUp() {
        var list = [BigInt(1), BigInt(2), BigInt(3)];
        this.jsPrimitiveList = {list: list};
    }
    testJavaPrimitiveListToCpp()
    {
        assertTrue(this.m.TestHelpers.checkPrimitiveList(this.jsPrimitiveList));
    }

    testCppPrimitiveListToJava()
    {
        var converted = this.m.TestHelpers.getPrimitiveList();
        assertArrayEq(converted.list, this.jsPrimitiveList.list);
    }

    testBinary()
    {
        var b = new Uint8Array([1, 2, 3]);
        assertArrayEq(this.m.TestHelpers.idBinary(b), b);
    }
}
allTests.push(PrimitiveListTest);

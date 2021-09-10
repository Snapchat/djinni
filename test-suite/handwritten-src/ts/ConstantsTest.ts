import {TestCase, allTests, assertEq} from "./testutils"
import * as test from "../../generated-src/ts/test";

export class ConstantsTest extends TestCase {
    constructor(module: any) {
        super(module);
    }
    testConstPrimitives() {
        assertEq(test.Constants.I8_CONSTANT, 1);
        assertEq(test.Constants.OPT_I8_CONSTANT, 1);
        assertEq(test.Constants.STRING_CONSTANT, "string-constant");
    }
    testConstRecords() {
        assertEq(test.Constants.OBJECT_CONSTANT, {
            someInteger: 3,
            someString: "string-constant"
        });
    }
}

allTests.push(ConstantsTest);

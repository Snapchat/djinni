import {TestCase, allTests, assertEq} from "./testutils"
import * as test from "../../generated-src/ts/test";
import {DjinniModule} from "@djinni_support/DjinniModule"

class CppExceptionTest extends TestCase  {
    m: test.Test_statics & DjinniModule;
    cppInterface: test.CppException;

    constructor(module: test.Test_statics & DjinniModule) {
        super(module);
        this.m = module;
        this.cppInterface = this.m.testsuite.CppException.get()!;
    }

    testCppException() {
        var thrown = null;
        try {
            this.cppInterface.throwAnException();
        } catch (e: any) {
            thrown = this.m.getExceptionMessage(e);
        }
        assertEq(thrown, "Exception Thrown");
    }
}

allTests.push(CppExceptionTest);

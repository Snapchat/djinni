import {TestCase, allTests, assertEq, assertTrue} from "./testutils"
import * as test from "../../generated-src/ts/test";
import {DjinniModule} from "@djinni_support/DjinniModule"

class TsThrower implements test.JsException {
    throwJsException() {
        throw new Error("TS Error");
    }
}

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
            thrown = e;
        }
        assertTrue(thrown instanceof Error);
        assertEq(thrown.message, "djinni: Exception Thrown");
    }

    testTsExceptionPassthrough() {
        var thrown = null;
        try {
            this.cppInterface.throwJsException(new TsThrower());
        } catch (e: any) {
            thrown = e;
        }
        assertTrue(thrown instanceof Error);
        assertEq(thrown.message, "TS Error");
    }
}

allTests.push(CppExceptionTest);

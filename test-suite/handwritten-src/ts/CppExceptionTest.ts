import {TestCase, allTests, assertEq, fail, assertTrue} from "./testutils"
import * as test from "../../generated-src/ts/test";
import {DjinniModule} from "@djinni_support/DjinniModule"

class CppExceptionTest extends TestCase  {
    m: test.Test_statics & DjinniModule;
    cppInterface: test.CppException;

    constructor(module: test.Test_statics & DjinniModule) {
        super(module);
        this.m = module;
    }

    setUp() {
        this.cppInterface = this.m.CppException.get();
    }

    testCppException() {
        try {
            this.cppInterface.throwAnException();
            fail();
        } catch (e) {
            var thrown = this.m.getExceptionMessage(e);
            assertEq(thrown, "Exception Thrown");
        }
    }

    testJsException() {
        try {
            this.cppInterface.throwAnExceptionFromJs({
                doThrow: function() {
                    throw new Error("js_exception_thrown");
                }
            });
            fail();
        } catch (e) {
            assertTrue(e instanceof Error)
            assertEq(e.message, "js_exception_thrown");
        }
    }
}

allTests.push(CppExceptionTest);

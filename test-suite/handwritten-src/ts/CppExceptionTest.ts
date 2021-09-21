import {TestCase, allTests, assertEq} from "./testutils"
import * as test from "../../generated-src/ts/test";

class CppExceptionTest extends TestCase  {
    m: test.Module_statics;
    cppInterface: test.CppException;

    constructor(module: test.Module_statics) {
        super(module);
        this.m = module;
    }

    setUp() {
        this.cppInterface = this.m.CppException.get();
    }

    testCppException() {
        var thrown = null;
        try {
            this.cppInterface.throwAnException();
        } catch (e) {
            thrown = e;
        }
        assertEq(thrown, new Error("Exception Thrown"));
    }
}

allTests.push(CppExceptionTest);

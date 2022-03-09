import {TestCase, allTests, assertEq, assertTrue} from "./testutils"
import * as test from "../../generated-src/ts/test";
import {DjinniModule} from "@djinni_support/DjinniModule"

class TsThrower implements test.ThrowingInterface {
    throwException() {
        throw new Error("TS Error");
    }
}

class CppThrower implements test.ThrowingInterface {
    cpp: test.CppException;
    constructor(cpp: test.CppException) {
        this.cpp = cpp;
    }
    throwException() {
        this.cpp.throwAnException();
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

    // C++ exception can be handled in TS
    testCppException() {
        var thrown = null;
        try {
            this.cppInterface.throwAnException();
        } catch (e: any) {
            thrown = e;
        }
        assertTrue(thrown instanceof Error);
        assertEq(thrown.message, "C++: Exception Thrown");
    }

    // TS exception passes through C++ code and handled in TS higher up in the stack
    testTsExceptionPassthrough() {
        var thrown = null;
        try {
            this.cppInterface.callThrowingInterface(new TsThrower());
        } catch (e: any) {
            thrown = e;
        }
        assertTrue(thrown instanceof Error);
        assertEq(thrown.message, "TS Error");
    }

    // TS exception can be handled in C++
    testTsExceptionHandledByCpp() {
        let res = this.cppInterface.callThrowingAndCatch(new TsThrower());
        // the stack info from the js exception
        assertTrue(res.length > 0 );
    }

    // C++ exception passes through TS code and handled in C++ higher up in the stack
    testCppExceptionPassthrough() {
        let res = this.cppInterface.callThrowingAndCatch(new CppThrower(this.cppInterface));
        assertEq(res, "Exception Thrown");
    }
}

allTests.push(CppExceptionTest);

class JsThrower {
    throwException() {
        throw new Error("JS Error");
    }
}

class CppThrower {
    constructor(cpp) {
        this.cpp = cpp;
    }
    throwException() {
        this.cpp.throwAnException();
    }
}

class CppExceptionTest {
    constructor(module) {
        this.m = module;
    }

    setUp() {
        this.cppInterface = this.m.testsuite.CppException.get();
    }

    // C++ exception can be handled in JS
    testCppException() {
        var thrown = null;
        try {
            this.cppInterface.throwAnException();
        } catch (e) {
            thrown = e;
        }
        assertEq(thrown.message, "C++: Exception Thrown");
    }

    // JS exception passes through C++ code and handled in JS higher up in the stack
    testJsExceptionPassthrough() {
        var thrown = null;
        try {
            this.cppInterface.callThrowingInterface(new JsThrower());
        } catch (e) {
            thrown = e;
        }
        assertTrue(thrown instanceof Error);
        assertEq(thrown.message, "JS Error");
    }
    // JS exception can be handled in C++
    testJsExceptionHandledByCpp() {
        let res = this.cppInterface.callThrowingAndCatch(new JsThrower());
        // the stack info from the js exception
        assertTrue(res.length > 0 );
    }

    // C++ exception passes through JS code and handled in C++ higher up in the stack
    testCppExceptionPassthrough() {
        let res = this.cppInterface.callThrowingAndCatch(new CppThrower(this.cppInterface));
        assertEq(res, "Exception Thrown");
    }
}

allTests.push(CppExceptionTest);

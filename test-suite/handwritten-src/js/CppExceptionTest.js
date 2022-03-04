class JsThrower {
    throwException() {
        throw new Error("JS Error");
    }
}

class CppExceptionTest {
    constructor(module) {
        this.m = module;
    }

    setUp() {
        this.cppInterface = this.m.testsuite.CppException.get();
    }

    testCppException() {
        var thrown = null;
        try {
            this.cppInterface.throwAnException();
        } catch (e) {
            thrown = e;
        }
        assertEq(thrown.message, "C++: Exception Thrown");
    }

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
}

allTests.push(CppExceptionTest);

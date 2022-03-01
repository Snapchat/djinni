class JsThrower {
    throwJsException() {
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
        assertEq(thrown.message, "djinni: Exception Thrown");
    }

    testJsExceptionPassthrough() {
        var thrown = null;
        try {
            this.cppInterface.throwJsException(new JsThrower());
        } catch (e) {
            thrown = e;
        }
        assertTrue(thrown instanceof Error);
        assertEq(thrown.message, "JS Error");
    }
}

allTests.push(CppExceptionTest);

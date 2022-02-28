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
}

allTests.push(CppExceptionTest);

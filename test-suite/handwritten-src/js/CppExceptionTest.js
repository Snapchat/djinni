class CppExceptionTest {
    constructor(module) {
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
        // In JS all we get is a number (probably a pointer)
        // Don't know how to get to the actual exception object.
        assertNe(thrown, null);
    }
}

allTests.push(CppExceptionTest);

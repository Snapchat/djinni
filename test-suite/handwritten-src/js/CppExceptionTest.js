class CppExceptionTest {
    constructor(module) {
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
            assertNe(thrown, null);
        }
    }

    testCppExceptionFromJs() {
        try {
            this.cppInterface.throwAnExceptionFromJs({
                doThrow: function() {
                    throw new Error("js_exception_thrown");
                }
            });
            fail();
        } catch (e) {
            assertTrue(e instanceof Error);
            assertEq(e.message, "js_exception_thrown");
        }
    }
}

allTests.push(CppExceptionTest);

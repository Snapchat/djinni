class ConstantsTest {
    constructor(module) {
        this.m = module;
    }
    testConstPrimitives() {
        assertEq(this.m.testsuite.Constants.I8_CONSTANT, 1);
        assertEq(this.m.testsuite.Constants.OPT_I8_CONSTANT, 1);
        assertEq(this.m.testsuite.Constants.STRING_CONSTANT, "string-constant");
    }
    testConstRecords() {
        assertEq(this.m.testsuite.Constants.OBJECT_CONSTANT, {
            someInteger: 3,
            someString: "string-constant"
        });
    }
}

allTests.push(ConstantsTest);

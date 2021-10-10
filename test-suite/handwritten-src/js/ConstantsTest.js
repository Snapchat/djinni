class ConstantsTest {
    constructor(module) {
        this.m = module;
    }
    testConstPrimitives() {
        assertEq(this.m.Constants.I8_CONSTANT, 1);
        assertEq(this.m.Constants.OPT_I8_CONSTANT, 1);
        assertEq(this.m.Constants.STRING_CONSTANT, "string-constant");
    }
    testConstRecords() {
        assertEq(this.m.Constants.OBJECT_CONSTANT, {
            someInteger: 3,
            someString: "string-constant"
        });
    }
}

allTests.push(ConstantsTest);

class WcharTest {
    constructor(module) {
        this.m = module;
    }

    static STR1 = "some string with unicode \u{0000}, \u{263A}, \u{D83D}\u{DCA9} symbols";
    static STR2 = "another string with unicode \u{263B}, \u{D83D}\u{DCA8} symbols";

    test() {
        assertEq(this.m.testsuite.WcharTestHelpers.getRecord().s, WcharTest.STR1);
        assertEq(this.m.testsuite.WcharTestHelpers.getString(), WcharTest.STR2);
        assertEq(this.m.testsuite.WcharTestHelpers.checkString(WcharTest.STR2), true);
        assertEq(this.m.testsuite.WcharTestHelpers.checkRecord({s: WcharTest.STR1}), true);
    }
}

allTests.push(WcharTest);

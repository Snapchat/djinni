class EnumTest {
    constructor(module) {
        this.module = module;
    }
    testEnumKey() {
        var m = new Map();
        m.set(this.module.Color.RED, "red");
        m.set(this.module.Color.ORANGE, "orange");
        m.set(this.module.Color.YELLOW, "yellow");
        m.set(this.module.Color.GREEN, "green");
        m.set(this.module.Color.BLUE, "blue");
        m.set(this.module.Color.INDIGO, "indigo");
        m.set(this.module.Color.VIOLET, "violet");
        this.module.TestHelpers.checkEnumMap(m);
    }
    testAccessFlagRoundtrip() {
        assertTrue(false);
    }
    testEmptyFlagRoundtrip() {
        assertTrue(false);
    }
}

allTests.push(EnumTest);

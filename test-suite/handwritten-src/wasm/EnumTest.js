class EnumTest {
    constructor(module) {
        this.m = module;
    }
    testEnumKey() {
        var map = new Map();
        map.set(this.m.Color.RED, "red");
        map.set(this.m.Color.ORANGE, "orange");
        map.set(this.m.Color.YELLOW, "yellow");
        map.set(this.m.Color.GREEN, "green");
        map.set(this.m.Color.BLUE, "blue");
        map.set(this.m.Color.INDIGO, "indigo");
        map.set(this.m.Color.VIOLET, "violet");
        this.m.TestHelpers.checkEnumMap(map);
    }
    testAccessFlagRoundtrip() {
        var flags = [
            this.m.AccessFlags.NOBODY,
            this.m.AccessFlags.EVERYBODY,
            this.m.AccessFlags.OWNER_READ,
            this.m.AccessFlags.OWNER_READ | this.m.AccessFlags.OWNER_WRITE,
            this.m.AccessFlags.OWNER_READ | this.m.AccessFlags.OWNER_WRITE | this.m.AccessFlags.OWNER_EXECUTE,
        ];
        for (var i = 0; i < flags.length; ++i) {
            assertEq(flags[i], this.m.FlagRoundtrip.roundtripAccess(flags[i]));
            assertEq(flags[i], this.m.FlagRoundtrip.roundtripAccessBoxed(flags[i]));
        }
        assertNull(this.m.FlagRoundtrip.roundtripAccessBoxed(null));
    }
    testEmptyFlagRoundtrip() {
        var flags = [
            this.m.EmptyFlags.NONE,
            this.m.EmptyFlags.ALL,
        ];
        for (var i = 0; i < flags.length; ++i) {
            assertEq(flags[i], this.m.FlagRoundtrip.roundtripEmpty(flags[i]));
            assertEq(flags[i], this.m.FlagRoundtrip.roundtripEmptyBoxed(flags[i]));
        }
        assertNull(this.m.FlagRoundtrip.roundtripEmptyBoxed(null));
    }
}

allTests.push(EnumTest);

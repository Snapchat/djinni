class EnumTest {
    constructor(module) {
        this.m = module;
    }

    enumToString(v, e) {
        for (var k in e) {
            if (e[k] == v) {
                return k;
            }
        }
        return v;
    }
    
    testEnumToString() {
        assertEq(this.enumToString(this.m.Color.RED, this.m.Color), "RED");
        assertEq(this.enumToString(this.m.Color.ORANGE, this.m.Color), "ORANGE");
        assertEq(this.enumToString(this.m.Color.YELLOW, this.m.Color), "YELLOW");
        assertEq(this.enumToString(this.m.Color.GREEN, this.m.Color), "GREEN");
        assertEq(this.enumToString(this.m.Color.BLUE, this.m.Color), "BLUE");
        assertEq(this.enumToString(this.m.Color.INDIGO, this.m.Color), "INDIGO");
        assertEq(this.enumToString(this.m.Color.VIOLET, this.m.Color), "VIOLET");
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
        assertUndefined(this.m.FlagRoundtrip.roundtripAccessBoxed(null));
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
        assertUndefined(this.m.FlagRoundtrip.roundtripEmptyBoxed(null));
    }
}

allTests.push(EnumTest);

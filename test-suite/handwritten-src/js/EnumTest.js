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
        assertEq(this.enumToString(this.m.testsuite.Color.RED, this.m.testsuite.Color), "RED");
        assertEq(this.enumToString(this.m.testsuite.Color.ORANGE, this.m.testsuite.Color), "ORANGE");
        assertEq(this.enumToString(this.m.testsuite.Color.YELLOW, this.m.testsuite.Color), "YELLOW");
        assertEq(this.enumToString(this.m.testsuite.Color.GREEN, this.m.testsuite.Color), "GREEN");
        assertEq(this.enumToString(this.m.testsuite.Color.BLUE, this.m.testsuite.Color), "BLUE");
        assertEq(this.enumToString(this.m.testsuite.Color.INDIGO, this.m.testsuite.Color), "INDIGO");
        assertEq(this.enumToString(this.m.testsuite.Color.VIOLET, this.m.testsuite.Color), "VIOLET");
    }
    
    testEnumKey() {
        var map = new Map();
        map.set(this.m.testsuite.Color.RED, "red");
        map.set(this.m.testsuite.Color.ORANGE, "orange");
        map.set(this.m.testsuite.Color.YELLOW, "yellow");
        map.set(this.m.testsuite.Color.GREEN, "green");
        map.set(this.m.testsuite.Color.BLUE, "blue");
        map.set(this.m.testsuite.Color.INDIGO, "indigo");
        map.set(this.m.testsuite.Color.VIOLET, "violet");
        this.m.testsuite.TestHelpers.checkEnumMap(map);
    }
    testAccessFlagRoundtrip() {
        var flags = [
            this.m.testsuite.AccessFlags.NOBODY,
            this.m.testsuite.AccessFlags.EVERYBODY,
            this.m.testsuite.AccessFlags.OWNER_READ,
            this.m.testsuite.AccessFlags.OWNER_READ | this.m.testsuite.AccessFlags.OWNER_WRITE,
            this.m.testsuite.AccessFlags.OWNER_READ | this.m.testsuite.AccessFlags.OWNER_WRITE | this.m.testsuite.AccessFlags.OWNER_EXECUTE,
        ];
        for (var i = 0; i < flags.length; ++i) {
            assertEq(flags[i], this.m.testsuite.FlagRoundtrip.roundtripAccess(flags[i]));
            assertEq(flags[i], this.m.testsuite.FlagRoundtrip.roundtripAccessBoxed(flags[i]));
        }
        assertUndefined(this.m.testsuite.FlagRoundtrip.roundtripAccessBoxed(null));
    }
    testEmptyFlagRoundtrip() {
        var flags = [
            this.m.testsuite.EmptyFlags.NONE,
            this.m.testsuite.EmptyFlags.ALL,
        ];
        for (var i = 0; i < flags.length; ++i) {
            assertEq(flags[i], this.m.testsuite.FlagRoundtrip.roundtripEmpty(flags[i]));
            assertEq(flags[i], this.m.testsuite.FlagRoundtrip.roundtripEmptyBoxed(flags[i]));
        }
        assertUndefined(this.m.testsuite.FlagRoundtrip.roundtripEmptyBoxed(null));
    }
}

allTests.push(EnumTest);

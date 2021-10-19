import {TestCase, allTests, assertEq, assertNull} from "./testutils"
import * as test from "../../generated-src/ts/test";

class EnumTest extends TestCase {
    m: test.Test_statics;
    constructor(module: any) {
        super(module);
        this.m = <test.Test_statics>module;
    }

    enumToString(v: test.Color) {
        return test.Color[v];
    }
    
    testEnumToString() {
        assertEq(this.enumToString(test.Color.RED), "RED");
        assertEq(this.enumToString(test.Color.ORANGE), "ORANGE");
        assertEq(this.enumToString(test.Color.YELLOW), "YELLOW");
        assertEq(this.enumToString(test.Color.GREEN), "GREEN");
        assertEq(this.enumToString(test.Color.BLUE), "BLUE");
        assertEq(this.enumToString(test.Color.INDIGO), "INDIGO");
        assertEq(this.enumToString(test.Color.VIOLET), "VIOLET");
    }
    
    testEnumKey() {
        var map = new Map();
        map.set(test.Color.RED, "red");
        map.set(test.Color.ORANGE, "orange");
        map.set(test.Color.YELLOW, "yellow");
        map.set(test.Color.GREEN, "green");
        map.set(test.Color.BLUE, "blue");
        map.set(test.Color.INDIGO, "indigo");
        map.set(test.Color.VIOLET, "violet");
        this.m.TestHelpers.checkEnumMap(map);
    }
    testAccessFlagRoundtrip() {
        var flags = [
            test.AccessFlags.NOBODY,
            test.AccessFlags.EVERYBODY,
            test.AccessFlags.OWNER_READ,
            test.AccessFlags.OWNER_READ | test.AccessFlags.OWNER_WRITE,
            test.AccessFlags.OWNER_READ | test.AccessFlags.OWNER_WRITE | test.AccessFlags.OWNER_EXECUTE,
        ];
        for (var i = 0; i < flags.length; ++i) {
            assertEq(flags[i], this.m.FlagRoundtrip.roundtripAccess(flags[i]));
            assertEq(flags[i], this.m.FlagRoundtrip.roundtripAccessBoxed(flags[i]));
        }
        assertNull(this.m.FlagRoundtrip.roundtripAccessBoxed(null));
    }
    testEmptyFlagRoundtrip() {
        var flags = [
            test.EmptyFlags.NONE,
            test.EmptyFlags.ALL,
        ];
        for (var i = 0; i < flags.length; ++i) {
            assertEq(flags[i], this.m.FlagRoundtrip.roundtripEmpty(flags[i]));
            assertEq(flags[i], this.m.FlagRoundtrip.roundtripEmptyBoxed(flags[i]));
        }
        assertNull(this.m.FlagRoundtrip.roundtripEmptyBoxed(null));
    }
}

allTests.push(EnumTest);

import {TestCase, allTests, assertEq} from "./testutils"
import * as test from "../../generated-src/ts/test";

class PrimitivesTest extends TestCase {
    m: test.Test_statics;
    constructor(module: test.Test_module_statics) {
        super(module);
        this.m = module.testsuite;
    }
    testPrimitives() {
        var p = {
            b: true,                         // bool
            eight: 123,                      // i8
            sixteen: 20000,                  // i16
            thirtytwo: 1000000000,           // i32
            sixtyfour: BigInt("1234567890123456789"), // i64
            fthirtytwo: 0,                   // f32
            fsixtyfour: 1.23,                // f64

            oB: true,                         // optional<bool>
            oEight: 123,                      // optional<i8>
            oSixteen: 20000,                  // optional<i16>
            oThirtytwo: 1000000000,           // optional<i32>
            oSixtyfour: BigInt("1234567890123456789"), // optional<i64>
            oFthirtytwo: 0,                   // optional<f32>
            oFsixtyfour: 1.23                 // optional<f64>
        };
        // f32 roundtrip test is not applicable to js because js has no native
        // 32-bit floating point number.
        assertEq(p, this.m.TestHelpers.assortedPrimitivesId(p));
    }
}
allTests.push(PrimitivesTest);

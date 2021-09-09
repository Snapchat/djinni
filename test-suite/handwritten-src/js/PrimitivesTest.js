class PrimitivesTest {
    constructor(module) {
        this.m = module;
    }
    testPrimitives() {
        var p = {
            b: true,                         // bool
            eight: 123,                      // i8
            sixteen: 20000,                  // i16
            thirtytwo: 1000000000,           // i32
            sixtyfour: 1234567890123456789n, // i64
            fthirtytwo: 0,                   // f32
            fsixtyfour: 1.23,                // f64

            oB: true,                         // optional<bool>
            oEight: 123,                      // optional<i8>
            oSixteen: 20000,                  // optional<i16>
            oThirtytwo: 1000000000,           // optional<i32>
            oSixtyfour: 1234567890123456789n, // optional<i64>
            oFthirtytwo: 0,                   // optional<f32>
            oFsixtyfour: 1.23                 // optional<f64>
        };
        // f32 roundtrip test is not applicable to js because js has no native
        // 32-bit floating point number.
        assertEq(p, this.m.TestHelpers.assortedPrimitivesId(p));
    }
}
allTests.push(PrimitivesTest);

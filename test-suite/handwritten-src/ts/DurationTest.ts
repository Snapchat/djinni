import {TestCase, allTests, assertEq, assertUndefined} from "./testutils"
import * as test from "../../generated-src/ts/test";

function nanos(x: number) { return x / 1000000; }
function micros(x: number) { return x / 1000; }
function millis(x: number) { return x; }
function seconds(x: number) { return x * 1000; }
function minutes(x: number) { return x * 1000 * 60; }
function hours(x: number) { return x * 1000 * 3600; }

class DurationTest extends TestCase {
    m: test.Test_statics;
    constructor(module: test.Test_statics) {
        super(module);
        this.m = module;
    }
    test() {
        assertEq(this.m.testsuite.TestDuration.hoursString(hours(1)), "1");
        assertEq(this.m.testsuite.TestDuration.minutesString(minutes(1)), "1");
        assertEq(this.m.testsuite.TestDuration.secondsString(seconds(1)), "1");
        assertEq(this.m.testsuite.TestDuration.millisString(millis(1)), "1");
        assertEq(this.m.testsuite.TestDuration.microsString(micros(1)), "1");
        assertEq(this.m.testsuite.TestDuration.nanosString(nanos(1)), "1");

        assertEq(this.m.testsuite.TestDuration.hours(1), hours(1));
        assertEq(this.m.testsuite.TestDuration.minutes(1), minutes(1));
        assertEq(this.m.testsuite.TestDuration.seconds(1), seconds(1));
        assertEq(this.m.testsuite.TestDuration.millis(1), millis(1));
        assertEq(this.m.testsuite.TestDuration.micros(1), micros(1));
        assertEq(this.m.testsuite.TestDuration.nanos(1), nanos(1));

        assertEq(this.m.testsuite.TestDuration.hoursf(1.5), minutes(90));
        assertEq(this.m.testsuite.TestDuration.minutesf(1.5), seconds(90));
        assertEq(this.m.testsuite.TestDuration.secondsf(1.5), millis(1500));
        assertEq(this.m.testsuite.TestDuration.millisf(1.5), micros(1500));
        assertEq(this.m.testsuite.TestDuration.microsf(1.5), nanos(1500));
        assertEq(this.m.testsuite.TestDuration.nanosf(1.0), nanos(1));

        assertEq(this.m.testsuite.TestDuration.box(BigInt(1)), seconds(1));
        assertUndefined(this.m.testsuite.TestDuration.box(BigInt(-1)));

        assertEq(this.m.testsuite.TestDuration.unbox(seconds(1)), 1);
        assertEq(this.m.testsuite.TestDuration.unbox(null), -1);
    }
}

allTests.push(DurationTest);

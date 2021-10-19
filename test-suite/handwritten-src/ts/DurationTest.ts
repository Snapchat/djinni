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
    constructor(module: any) {
        super(module);
        this.m = <test.Test_statics>module;
    }
    test() {
        assertEq(this.m.TestDuration.hoursString(hours(1)), "1");
        assertEq(this.m.TestDuration.minutesString(minutes(1)), "1");
        assertEq(this.m.TestDuration.secondsString(seconds(1)), "1");
        assertEq(this.m.TestDuration.millisString(millis(1)), "1");
        assertEq(this.m.TestDuration.microsString(micros(1)), "1");
        assertEq(this.m.TestDuration.nanosString(nanos(1)), "1");

        assertEq(this.m.TestDuration.hours(1), hours(1));
        assertEq(this.m.TestDuration.minutes(1), minutes(1));
        assertEq(this.m.TestDuration.seconds(1), seconds(1));
        assertEq(this.m.TestDuration.millis(1), millis(1));
        assertEq(this.m.TestDuration.micros(1), micros(1));
        assertEq(this.m.TestDuration.nanos(1), nanos(1));

        assertEq(this.m.TestDuration.hoursf(1.5), minutes(90));
        assertEq(this.m.TestDuration.minutesf(1.5), seconds(90));
        assertEq(this.m.TestDuration.secondsf(1.5), millis(1500));
        assertEq(this.m.TestDuration.millisf(1.5), micros(1500));
        assertEq(this.m.TestDuration.microsf(1.5), nanos(1500));
        assertEq(this.m.TestDuration.nanosf(1.0), nanos(1));

        assertEq(this.m.TestDuration.box(BigInt(1)), seconds(1));
        assertUndefined(this.m.TestDuration.box(BigInt(-1)));

        assertEq(this.m.TestDuration.unbox(seconds(1)), 1);
        assertEq(this.m.TestDuration.unbox(null), -1);
    }
}

allTests.push(DurationTest);

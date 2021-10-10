import {TestCase, allTests, assertEq, assertNe} from "./testutils"
import * as test from "../../generated-src/ts/test";

class OutcomeTest extends TestCase {
    m: test.Module_statics;
    constructor(module: any) {
        super(module);
        this.m = <test.Module_statics>module;
    }
    test() {
        // construct result outcome in native and pass to js
        var r = this.m.TestOutcome.getSuccessOutcome();
        assertEq(r, {result: "hello"});

        // construct error outcome in native and pass to js
        var e = this.m.TestOutcome.getErrorOutcome();
        assertEq(e, {error: 42});

        // result outcome and error outcome does not compare equal
        assertNe(r, e);
        
        // construct result outcome in js then pass to native and back
        assertEq(this.m.TestOutcome.putSuccessOutcome({result:"hello"}), "hello");
        // construct error outcome in js then pass to native and back
        assertEq(this.m.TestOutcome.putErrorOutcome({error:42}), 42);

        // Hash equal tests are not applicable -
        // Javascript does not support overloading the hash method

        // test outcome as nested object
        var nr = this.m.TestOutcome.getNestedSuccessOutcome();
        assertEq(nr.o, {result: 42});

        var ne = this.m.TestOutcome.getNestedErrorOutcome();
        assertEq(ne.o, {error: "hello"});

        assertEq(this.m.TestOutcome.putNestedSuccessOutcome({o: {result:42}}), 42);
        assertEq(this.m.TestOutcome.putNestedErrorOutcome({o: {error:"hello"}}), "hello");
    }
}

allTests.push(OutcomeTest);

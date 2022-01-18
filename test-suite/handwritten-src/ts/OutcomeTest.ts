import {TestCase, allTests, assertEq, assertNe} from "./testutils"
import * as test from "../../generated-src/ts/test";

class OutcomeTest extends TestCase {
    m: test.Test_statics;
    constructor(module: test.Test_statics) {
        super(module);
        this.m = module;
    }
    test() {
        // construct result outcome in native and pass to js
        var r = this.m.testsuite.TestOutcome.getSuccessOutcome();
        assertEq(r, {result: "hello"});

        // construct error outcome in native and pass to js
        var e = this.m.testsuite.TestOutcome.getErrorOutcome();
        assertEq(e, {error: 42});

        // result outcome and error outcome does not compare equal
        assertNe(r, e);
        
        // construct result outcome in js then pass to native and back
        assertEq(this.m.testsuite.TestOutcome.putSuccessOutcome({result:"hello"}), "hello");
        // construct error outcome in js then pass to native and back
        assertEq(this.m.testsuite.TestOutcome.putErrorOutcome({error:42}), 42);

        // Hash equal tests are not applicable -
        // Javascript does not support overloading the hash method

        // test outcome as nested object
        var nr = this.m.testsuite.TestOutcome.getNestedSuccessOutcome();
        assertEq(nr.o, {result: 42});

        var ne = this.m.testsuite.TestOutcome.getNestedErrorOutcome();
        assertEq(ne.o, {error: "hello"});

        assertEq(this.m.testsuite.TestOutcome.putNestedSuccessOutcome({o: {result:42}}), 42);
        assertEq(this.m.testsuite.TestOutcome.putNestedErrorOutcome({o: {error:"hello"}}), "hello");
    }
}

allTests.push(OutcomeTest);

package com.dropbox.djinni.test;

import junit.framework.TestCase;
import com.snapchat.djinni.Outcome;

public class OutcomeTest extends TestCase {
    public void test() {
        // construct result outcome in native and pass to objc
        Outcome r = TestOutcome.getSuccessOutcome();
        // results are equal
        assertEquals(r.result().get(), "hello");
        // outcome objects compare equal
        assertEquals(r, Outcome.fromResult("hello"));

        // construct error outcome in native and pass to objc
        Outcome e = TestOutcome.getErrorOutcome();
        // error values are equal
        assertEquals(e.error().get(), 42);
        // outcome objects compare equal
        assertEquals(e, Outcome.fromError(42));

        // result outcome and error outcome does not compare equal
        assertFalse(r.equals(e));

        // construct result outcome in objc then pass to native and back
        assertEquals(TestOutcome.putSuccessOutcome(Outcome.fromResult("hello")), "hello");
        // construct error outcome in objc then pass to native and back
        assertEquals(TestOutcome.putErrorOutcome(Outcome.fromError(42)), 42);

        // Hash equal
        assertEquals(r.hashCode(), Outcome.fromResult("hello").hashCode());
        assertEquals(e.hashCode(), Outcome.fromError(42).hashCode());
        // Hash not equal between result and error
        assertFalse(r.hashCode() == Outcome.fromError("hello").hashCode());
        assertFalse(e.hashCode() == Outcome.fromResult(42).hashCode());


        // test outcome as nested object
        NestedOutcome nr = TestOutcome.getNestedSuccessOutcome();
        assertEquals(nr.getO().result().get(), Integer.valueOf(42));
        assertEquals(nr, new NestedOutcome(Outcome.fromResult(42)));

        NestedOutcome ne = TestOutcome.getNestedErrorOutcome();
        assertEquals(ne.getO().error().get(), "hello");
        assertEquals(ne, new NestedOutcome(Outcome.fromError("hello")));

        assertEquals(TestOutcome.putNestedSuccessOutcome(new NestedOutcome(Outcome.fromResult(42))), 42);
        assertEquals(TestOutcome.putNestedErrorOutcome(new NestedOutcome(Outcome.fromError("hello"))), "hello");
    }
}

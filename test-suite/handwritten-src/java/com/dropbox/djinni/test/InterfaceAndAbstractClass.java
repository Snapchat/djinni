package com.dropbox.djinni.test;

import junit.framework.TestCase;

public class InterfaceAndAbstractClass extends TestCase {

    static class TestJavaInterfaceOnlyImpl implements TestJavaInterfaceOnly {
        @Override
        public boolean testMethod() {
            return true;
        }
    }

    public void test() {
        // interface
        TestJavaInterfaceOnlyImpl impl1 = new TestJavaInterfaceOnlyImpl();
        assertEquals(impl1.testMethod(), true);

        // abstract class
        assertEquals(TestJavaAbstractClassOnly.testMethod(), true);
    }
}

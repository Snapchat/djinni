package com.dropbox.djinni.test;

import junit.framework.TestCase;

public class FunctionPrologueTest extends TestCase {

    public void testFunctionPrologue() {
        String result = FunctionPrologueHelper.foo();
        assertEquals(result, "FunctionPrologueHelper.foo");
    }

}

package com.dropbox.djinni.test;

import junit.framework.TestCase;
import static org.junit.Assert.*;
import com.snapchat.djinni.Promise;
import com.snapchat.djinni.Future;

public class AsyncTest extends TestCase {
    public void test() {
        Future<Integer> f = TestHelpers.getAsyncResult();
        f.then((res) -> {
                System.out.println("yes" + res);
            });
    }
}

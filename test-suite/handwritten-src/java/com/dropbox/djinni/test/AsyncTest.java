package com.dropbox.djinni.test;

import junit.framework.TestCase;
import static org.junit.Assert.*;
import com.snapchat.djinni.Promise;
import com.snapchat.djinni.Future;

public class AsyncTest extends TestCase {
    static class ResultHolder {
        Integer res;
    }
    public void test() {
        Future<Integer> f = TestHelpers.getAsyncResult();
        final ResultHolder r = new ResultHolder();
        f.then((res) -> {
                synchronized(r) {
                    r.res = res;
                    r.notifyAll();
                }
            });
        try {
            synchronized(r) {
                while(r.res == null) {
                    r.wait();
                }
            }
        } catch (InterruptedException e) {
        }
        assertEquals(r.res, Integer.valueOf(42));
    }
}

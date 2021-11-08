package com.dropbox.djinni.test;

import junit.framework.TestCase;
import static org.junit.Assert.*;
import com.snapchat.djinni.Promise;
import com.snapchat.djinni.Future;

public class AsyncTest extends TestCase {
    static class ResultHolder {
        Integer res;
    }

    static class JavaAsyncInterfaceImpl extends AsyncInterface {
        public Future<Integer> getAsyncResult() {
            final Promise<Integer> p = new Promise<Integer>();
            Future<Integer> f = p.getFuture();
            Thread thread = new Thread("New Thread") {
                    public void run(){
                        p.setValue(42);
                    }
                };
            thread.start();
            return f;
        }
    }
    
    public void testConsumeNativeFuture() {
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
        assertEquals(Integer.valueOf(42), r.res);
    }
    public void testConsumePlatformFuture() {
        int r = TestHelpers.checkAsyncInterface(new JavaAsyncInterfaceImpl());
        assertEquals(r, 42);
    }
}

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
        f.then((Integer i) -> {
                System.out.println(i.getClass().getName());
                return i.toString();
            }).then((String s) -> {
                    System.out.println(s.getClass().getName());
                    synchronized(r) {
                        r.res = Integer.parseInt(s);
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

    public void testPassingFuture() {
        final Promise<String> p = new Promise<String>();
        Future<String> f = p.getFuture();
        Future<Integer> f2 = f.then((s) -> {
                return Integer.parseInt(s);
            });
        Future<String> f3 = TestHelpers.passFuture(f2);
        p.setValue("36");
        assertEquals(f3.get(), "36");
    }
}

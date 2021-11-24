package com.dropbox.djinni.test;

import junit.framework.TestCase;
import static org.junit.Assert.*;
import com.snapchat.djinni.Promise;
import com.snapchat.djinni.Future;

public class AsyncTest extends TestCase {
    public void testConsumeNativeFuture() throws Throwable {
        Future<Integer> f = TestHelpers.getAsyncResult();
        Future<Integer> f2 = f.then((i) -> {
                System.out.println(i.get().getClass().getName());
                return i.get().toString();
            }).then((s) -> {
                    System.out.println(s.get().getClass().getName());
                    return Integer.parseInt(s.get());
                });
        String s = null;
        try {
            assertEquals(Integer.valueOf(42), f2.get());
        } catch (Throwable e) {
            s = e.getMessage();
        }
        assertEquals(s, "123");
    }

    public void testFutureRoundtrip() throws Throwable {
        final Promise<String> p = new Promise<String>();
        Future<String> f = p.getFuture();
        Future<Integer> f2 = f.then((s) -> {
                if (true) {
                    throw new Exception("123");
                }
                return Integer.parseInt(s.get());
            });
        Future<String> f3 = TestHelpers.futureRoundtrip(f2);
        p.setValue("36");
        String s = null;
        try {
            assertEquals(f3.get(), "36");
        } catch (Exception e) {
            s = e.getMessage();
        }
        assertEquals(s, "123");
    }
}

package com.dropbox.djinni.test;

import junit.framework.TestCase;
import static org.junit.Assert.*;
import com.snapchat.djinni.Promise;
import com.snapchat.djinni.Future;

public class AsyncTest extends TestCase {
    public void testConsumeNativeFuture() {
        Future<Integer> f = TestHelpers.getAsyncResult();
        Future<Integer> f2 = f.then((Integer i) -> {
                System.out.println(i.getClass().getName());
                return i.toString();
            }).then((String s) -> {
                    System.out.println(s.getClass().getName());
                    return Integer.parseInt(s);
                });
        assertEquals(Integer.valueOf(42), f2.get());
    }

    public void testFutureRoundtrip() {
        final Promise<String> p = new Promise<String>();
        Future<String> f = p.getFuture();
        Future<Integer> f2 = f.then((s) -> {
                return Integer.parseInt(s);
            });
        Future<String> f3 = TestHelpers.futureRoundtrip(f2);
        p.setValue("36");
        assertEquals(f3.get(), "36");
    }
}

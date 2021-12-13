package com.dropbox.djinni.test;

import junit.framework.TestCase;
import static org.junit.Assert.*;
import com.snapchat.djinni.Promise;
import com.snapchat.djinni.Future;
import java.util.concurrent.ExecutionException;
import rx.Single;

public class AsyncTest extends TestCase {

    static class AsyncInterfaceImpl extends AsyncInterface {
        public com.snapchat.djinni.Future<String> futureRoundtrip(com.snapchat.djinni.Future<Integer> f) {
            return f.then((i) -> {return i.get().toString();});
        }
    }
    
    public void testConsumeNativeFuture() throws Throwable {
        Future<Integer> f = TestHelpers.getAsyncResult();
        Future<Integer> f2 = f.then((i) -> {
                return i.get().toString();
            }).then((s) -> {
                    return Integer.parseInt(s.get());
                });
        assertEquals(Integer.valueOf(42), f2.get());
    }

    public void testFutureRoundtrip() throws Throwable {
        final Promise<String> p = new Promise<String>();
        Future<String> f = p.getFuture();
        Future<Integer> f2 = f.then((s) -> {
                return Integer.parseInt(s.get());
            });
        Future<String> f3 = TestHelpers.futureRoundtrip(f2);
        p.setValue("36");
        assertEquals(f3.get(), "36");
    }

    public void testFutureRoundtripWithException() throws Throwable {
        final Promise<String> p = new Promise<String>();
        Future<String> f = p.getFuture();
        Future<Integer> f2 = f.then((s) -> {
                if (true) {
                    throw new Exception("123");
                }
                return 0;
            });
        Future<String> f3 = TestHelpers.futureRoundtrip(f2);
        p.setValue("36");
        String s = null;
        try {
            f3.get();
        } catch (ExecutionException e) {
            s = e.getMessage();
        }
        assertEquals(s, "123");
    }

    public void testFutureRoundtripBackwards() throws Throwable {
        Future<String> s = TestHelpers.checkAsyncInterface(new AsyncInterfaceImpl());
        assertEquals(s.get(), "36");
    }

    public void testRx() throws Throwable {
        Future<Integer> f = TestHelpers.getAsyncResult();
        Single<Integer> s = Single.create(o -> f.then((i) -> {
                    try {
                        o.onSuccess(i.get());
                    } catch (Throwable e) {
                        o.onError(e);
                    }
                }));
        assertEquals(Integer.valueOf(42), s.toBlocking().value());
    }
    
    public void testRxFromFuture() throws Throwable {
        Future<Integer> f = TestHelpers.getAsyncResult();
        Single<Integer> s = Single.from(f);
        assertEquals(Integer.valueOf(42), s.toBlocking().value());
    }
}

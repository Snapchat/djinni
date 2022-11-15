package com.dropbox.djinni.test;

import static org.junit.Assert.*;
import junit.framework.TestCase;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import io.reactivex.Single;

public class AsyncTest extends TestCase {

    static class AsyncInterfaceImpl extends AsyncInterface {
        public CompletableFuture<String> futureRoundtrip(CompletableFuture<Integer> f) {
            return f.thenApply((i) -> {return i.toString();});
        }
    }
    
    public void testConsumeNativeFuture() throws Throwable {
        CompletableFuture<Integer> f = TestHelpers.getAsyncResult();
        CompletableFuture<Integer> f2 = f.thenApply((i) -> {
                return i.toString();
            }).thenApply((s) -> {
                    return Integer.parseInt(s);
                });
        assertEquals(Integer.valueOf(42), f2.get());
    }

    public void testFutureRoundtrip() throws Throwable {
        final CompletableFuture<String> f = new CompletableFuture();
        CompletableFuture<Integer> f2 = f.thenApply((s) -> {
                return Integer.parseInt(s);
            });
        CompletableFuture<String> f3 = TestHelpers.futureRoundtrip(f2);
        f.complete("36");
        assertEquals(f3.get(), "36");
    }

    public void testFutureRoundtripWithException() throws Throwable {
        final CompletableFuture<String> f = new CompletableFuture();
        CompletableFuture<Integer> f2 = f.thenApply((s) -> {
                if (true) {
                    throw new RuntimeException("123");
                }
                return 0;
            });
        CompletableFuture<String> f3 = TestHelpers.futureRoundtrip(f2);
        f.complete("36");
        String s = null;
        try {
            f3.get();
        } catch (ExecutionException e) {
            s = e.getCause().getMessage();
        }
        assertEquals(s, "java.lang.RuntimeException: 123");
    }

    public void testFutureRoundtripBackwards() throws Throwable {
        CompletableFuture<String> s = TestHelpers.checkAsyncInterface(new AsyncInterfaceImpl());
        assertEquals(s.get(), "36");
    }

    public void testFutureComposition() throws Throwable {
        CompletableFuture<String> s = TestHelpers.checkAsyncComposition(new AsyncInterfaceImpl());
        assertEquals(s.get(), "42");
    }

    public void testRx() throws Throwable {
        CompletableFuture<Integer> f = TestHelpers.getAsyncResult();
        Single<Integer> s = Single.create(o -> f.whenComplete((i, e) -> {
                    if (e == null) {
                        o.onSuccess(i);
                    } else { 
                        o.onError(e);
                    }
                }));
        assertEquals(Integer.valueOf(42), s.blockingGet());
    }
    
    public void testRxFromFuture() throws Throwable {
        CompletableFuture<Integer> f = TestHelpers.getAsyncResult();
        Single<Integer> s = Single.fromFuture(f);
        assertEquals(Integer.valueOf(42), s.blockingGet());
    }
}

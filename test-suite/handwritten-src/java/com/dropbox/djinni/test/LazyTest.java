package com.dropbox.djinni.test;

import junit.framework.TestCase;
import com.google.common.base.Supplier;
import java.util.concurrent.atomic.AtomicInteger;

public class LazyTest extends TestCase {
    public void test() {
        // Simple type tests
        // get lazy string from native and call it
        Supplier<String> lazyStr = TestLazy.getLazyString();
        assertEquals(lazyStr.get(), "hello");

        // get lazy int from native and call it
        Supplier<Integer> lazyInt = TestLazy.getLazyInt();
        assertEquals(lazyInt.get(), Integer.valueOf(42));

        // create lazy string in java and pass to native which calls it
        Supplier<String> javaLazyStr = () -> "world";
        assertEquals(TestLazy.callLazyString(javaLazyStr), "world");

        // create lazy int in java and pass to native which calls it
        Supplier<Integer> javaLazyInt = () -> 123;
        assertEquals(123, TestLazy.callLazyInt(javaLazyInt));

        // test lazy as nested object
        NestedLazy nl = TestLazy.getNestedLazy();
        assertEquals(Integer.valueOf(99), nl.getL().get());

        assertEquals(55, TestLazy.callNestedLazy(new NestedLazy(() -> 55)));

        // Interface lazy tests - main use case for deferred expensive object creation
        // get lazy object from native and call it
        Supplier<SimpleObject> lazyObj = TestLazy.getLazyObject();
        SimpleObject obj = lazyObj.get();
        assertEquals(obj.getValue(), 42);
        assertEquals(obj.getName(), "expensive");

        // Verify lazy object is called from native
        assertEquals(TestLazy.callLazyObject(lazyObj), 42);
        assertEquals(TestLazy.callLazyObjectGetName(lazyObj), "expensive");

        // Create lazy object in Java and pass to native
        // Track that lazy creation is deferred
        final AtomicInteger creationCount = new AtomicInteger(0);
        Supplier<SimpleObject> javaLazyObj = () -> {
            creationCount.incrementAndGet();
            return TestLazy.createSimpleObject(999, "java-created");
        };
        
        // Object not created yet
        assertEquals(creationCount.get(), 0);
        
        // Call from native - object created on demand
        assertEquals(999, TestLazy.callLazyObject(javaLazyObj));
        assertEquals(1, creationCount.get());
        
        assertEquals("java-created", TestLazy.callLazyObjectGetName(javaLazyObj));
        assertEquals(2, creationCount.get()); // Created again

        // Test lazy interface as nested object
        NestedLazyInterface nli = TestLazy.getNestedLazyInterface();
        assertEquals(777, nli.getObj().get().getValue());

        assertEquals(888, TestLazy.callNestedLazyInterface(
            new NestedLazyInterface(() -> TestLazy.createSimpleObject(888, "nested-java"))
        ));

        // Test memoized supplier - verify factory is only called once
        final AtomicInteger memoCreationCount = new AtomicInteger(0);
        Supplier<SimpleObject> memoizedSupplier = memoize(() -> {
            memoCreationCount.incrementAndGet();
            return TestLazy.createSimpleObject(12345, "memoized");
        });

        // Not created yet
        assertEquals(0, memoCreationCount.get());

        // First call to C++ - creates the object
        assertEquals(12345, TestLazy.callLazyObject(memoizedSupplier));
        assertEquals(1, memoCreationCount.get());

        // Second call to C++ - should NOT create a new object (memoized)
        assertEquals("memoized", TestLazy.callLazyObjectGetName(memoizedSupplier));
        assertEquals(1, memoCreationCount.get()); // Still 1!

        // Third call to C++ - still memoized
        assertEquals(12345, TestLazy.callLazyObject(memoizedSupplier));
        assertEquals(1, memoCreationCount.get()); // Still 1!

        // Also test calling the supplier directly in Java
        SimpleObject memoObj = memoizedSupplier.get();
        assertEquals(12345, memoObj.getValue());
        assertEquals("memoized", memoObj.getName());
        assertEquals(1, memoCreationCount.get()); // Still 1!
    }

    /**
     * Helper to create a memoized (cached) supplier.
     * The factory is only called once, and the result is cached for subsequent calls.
     */
    private static <T> Supplier<T> memoize(Supplier<T> factory) {
        return new Supplier<T>() {
            private volatile T value;
            private volatile boolean initialized = false;

            @Override
            public T get() {
                if (!initialized) {
                    synchronized (this) {
                        if (!initialized) {
                            value = factory.get();
                            initialized = true;
                        }
                    }
                }
                return value;
            }
        };
    }
}

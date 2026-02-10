package com.dropbox.djinni.test;

import junit.framework.TestCase;
import com.google.common.base.Supplier;
import java.util.concurrent.atomic.AtomicInteger;

public class SupplierTest extends TestCase {
    public void test() {
        // Simple type tests
        // get supplier string from native and call it
        Supplier<String> supplierStr = TestSupplier.getSupplierString();
        assertEquals(supplierStr.get(), "hello");

        // get supplier int from native and call it
        Supplier<Integer> supplierInt = TestSupplier.getSupplierInt();
        assertEquals(supplierInt.get(), Integer.valueOf(42));

        // create supplier string in java and pass to native which calls it
        Supplier<String> javaSupplierStr = () -> "world";
        assertEquals(TestSupplier.callSupplierString(javaSupplierStr), "world");

        // create supplier int in java and pass to native which calls it
        Supplier<Integer> javaSupplierInt = () -> 123;
        assertEquals(123, TestSupplier.callSupplierInt(javaSupplierInt));

        // test supplier as nested object
        NestedSupplier ns = TestSupplier.getNestedSupplier();
        assertEquals(Integer.valueOf(99), ns.getS().get());

        assertEquals(55, TestSupplier.callNestedSupplier(new NestedSupplier(() -> 55)));

        // Interface supplier tests - main use case for deferred expensive object creation
        // get supplier object from native and call it
        Supplier<SimpleObject> supplierObj = TestSupplier.getSupplierObject();
        SimpleObject obj = supplierObj.get();
        assertEquals(obj.getValue(), 42);
        assertEquals(obj.getName(), "expensive");

        // Verify supplier object is called from native
        assertEquals(TestSupplier.callSupplierObject(supplierObj), 42);
        assertEquals(TestSupplier.callSupplierObjectGetName(supplierObj), "expensive");

        // Create supplier object in Java and pass to native
        // Track that creation is deferred
        final AtomicInteger creationCount = new AtomicInteger(0);
        Supplier<SimpleObject> javaSupplierObj = () -> {
            creationCount.incrementAndGet();
            return TestSupplier.createSimpleObject(999, "java-created");
        };

        // Object not created yet
        assertEquals(creationCount.get(), 0);

        // Call from native - object created on demand
        assertEquals(999, TestSupplier.callSupplierObject(javaSupplierObj));
        assertEquals(1, creationCount.get());

        assertEquals("java-created", TestSupplier.callSupplierObjectGetName(javaSupplierObj));
        assertEquals(2, creationCount.get()); // Created again

        // Test supplier interface as nested object
        NestedSupplierInterface nsi = TestSupplier.getNestedSupplierInterface();
        assertEquals(777, nsi.getObj().get().getValue());

        assertEquals(888, TestSupplier.callNestedSupplierInterface(
            new NestedSupplierInterface(() -> TestSupplier.createSimpleObject(888, "nested-java"))
        ));

        // Test memoized supplier - verify factory is only called once
        final AtomicInteger memoCreationCount = new AtomicInteger(0);
        Supplier<SimpleObject> memoizedSupplier = memoize(() -> {
            memoCreationCount.incrementAndGet();
            return TestSupplier.createSimpleObject(12345, "memoized");
        });

        // Not created yet
        assertEquals(0, memoCreationCount.get());

        // First call to C++ - creates the object
        assertEquals(12345, TestSupplier.callSupplierObject(memoizedSupplier));
        assertEquals(1, memoCreationCount.get());

        // Second call to C++ - should NOT create a new object (memoized)
        assertEquals("memoized", TestSupplier.callSupplierObjectGetName(memoizedSupplier));
        assertEquals(1, memoCreationCount.get()); // Still 1!

        // Third call to C++ - still memoized
        assertEquals(12345, TestSupplier.callSupplierObject(memoizedSupplier));
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

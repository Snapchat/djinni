package com.dropbox.djinni.test;

import junit.framework.TestCase;
import javax.inject.Provider;
import java.util.concurrent.atomic.AtomicInteger;

public class ProviderTest extends TestCase {
    public void test() {
        // Simple type tests
        Provider<String> providerStr = TestProvider.getProviderString();
        assertEquals(providerStr.get(), "hello");
        // C++ provider can be called more than once
        assertEquals(providerStr.get(), "hello");
        assertEquals(providerStr.get(), "hello");

        Provider<Integer> providerInt = TestProvider.getProviderInt();
        assertEquals(providerInt.get(), Integer.valueOf(42));
        // C++ provider can be called more than once
        assertEquals(providerInt.get(), Integer.valueOf(42));

        Provider<String> javaProviderStr = () -> "world";
        assertEquals(TestProvider.callProviderString(javaProviderStr), "world");
        // Host provider can be used in multiple C++ calls
        assertEquals(TestProvider.callProviderString(javaProviderStr), "world");

        Provider<Integer> javaProviderInt = () -> 123;
        assertEquals(123, TestProvider.callProviderInt(javaProviderInt));
        assertEquals(123, TestProvider.callProviderInt(javaProviderInt));

        NestedProvider np = TestProvider.getNestedProvider();
        assertEquals(Integer.valueOf(99), np.getS().get());

        assertEquals(55, TestProvider.callNestedProvider(new NestedProvider(() -> 55)));

        Provider<SimpleObject> providerObj = TestProvider.getProviderObject();
        SimpleObject obj = providerObj.get();
        assertEquals(obj.getValue(), 42);
        assertEquals(obj.getName(), "expensive");

        assertEquals(TestProvider.callProviderObject(providerObj), 42);
        assertEquals(TestProvider.callProviderObjectGetName(providerObj), "expensive");

        final AtomicInteger creationCount = new AtomicInteger(0);
        Provider<SimpleObject> javaProviderObj = () -> {
            creationCount.incrementAndGet();
            return TestProvider.createSimpleObject(999, "java-created");
        };

        assertEquals(creationCount.get(), 0);
        // C++ calls host provider twice to verify multi-call; expect 2 creations
        assertEquals(999, TestProvider.callProviderObject(javaProviderObj));
        assertEquals(2, creationCount.get());
        assertEquals("java-created", TestProvider.callProviderObjectGetName(javaProviderObj));
        assertEquals(4, creationCount.get());

        NestedProviderInterface npi = TestProvider.getNestedProviderInterface();
        assertEquals(777, npi.getObj().get().getValue());
        assertEquals(888, TestProvider.callNestedProviderInterface(
            new NestedProviderInterface(() -> TestProvider.createSimpleObject(888, "nested-java"))
        ));

        final AtomicInteger memoCreationCount = new AtomicInteger(0);
        Provider<SimpleObject> memoizedProvider = memoize(() -> {
            memoCreationCount.incrementAndGet();
            return TestProvider.createSimpleObject(12345, "memoized");
        });
        assertEquals(0, memoCreationCount.get());
        assertEquals(12345, TestProvider.callProviderObject(memoizedProvider));
        assertEquals(1, memoCreationCount.get());
        assertEquals("memoized", TestProvider.callProviderObjectGetName(memoizedProvider));
        assertEquals(1, memoCreationCount.get());
        assertEquals(12345, TestProvider.callProviderObject(memoizedProvider));
        assertEquals(1, memoCreationCount.get());
        SimpleObject memoObj = memoizedProvider.get();
        assertEquals(12345, memoObj.getValue());
        assertEquals("memoized", memoObj.getName());
        assertEquals(1, memoCreationCount.get());
    }

    private static <T> Provider<T> memoize(Provider<T> factory) {
        return new Provider<T>() {
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

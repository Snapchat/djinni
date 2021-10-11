/**
  * Copyright 2021 Snap, Inc.
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *    http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

package com.snapchat.djinni;

import java.lang.ref.PhantomReference;
import java.lang.ref.Reference;
import java.lang.ref.ReferenceQueue;
import java.lang.reflect.Method;

import java.util.concurrent.ConcurrentHashMap;

/**
 * Djinni used to generate finalizer methods for CppProxy objects. However,
 * Android can suspend the finalizer thread in the middle of a finalizer method,
 * and this causes the caller of the finalizer to produce a TimeoutException.
 *
 * In order to fix this, we make CppProxy objects register themselves with the
 * NativeObjectManager singleton in their constructors and remove the finalizers
 * from generated code.
 *
 * The NativeObjectManager uses Phantom References registered with
 * java.lang.ref.ReferenceQueue to track outstanding CppProxy objects. Whenever
 * a CppProxy object is ready for garbage collection, its phantom reference will
 * be popped from the ReferenceQueue by the `remove()` method. We then run the
 * CppProxy object's `nativeDestroy()` static method to destroy the native
 * counterpart.
 *
 * The remove/destroy loop runs in a dedicated low priority cleanup thread
 * outside of the Java GC.
 */
public class NativeObjectManager {

    // private ----------------------------------

    private static class Holder {
        static final NativeObjectManager instance = new NativeObjectManager();
    }

    private static class NativeObjectWrapper extends PhantomReference<Object> {

        private final long mNativeRef;
        private final Method mDestroyMethod;

        NativeObjectWrapper(Object referent, Class<?> clazz, long nativeRef, ReferenceQueue<? super Object> queue) throws NoSuchMethodException {
            super(referent, queue);
            mNativeRef = nativeRef;
            mDestroyMethod = clazz.getMethod("nativeDestroy", long.class);
        }

        String getClassName() {
            return mDestroyMethod.getDeclaringClass().getName();
        }

        void cleanup() throws Exception {
            mDestroyMethod.invoke(null, mNativeRef);
        }
    }

    private final ReferenceQueue<Object> mReferenceQueue = new ReferenceQueue<>();
    private final ConcurrentHashMap<NativeObjectWrapper, Boolean> mReferences = new ConcurrentHashMap<>();
    private final Thread mThread;

    private NativeObjectManager() {
        mThread = new Thread("NativeObjectManager") {
                public void run() {
                    try {
                        Reference<?> ref;
                        while ((ref = mReferenceQueue.remove()) != null) {
                            NativeObjectWrapper wrapper = (NativeObjectWrapper)ref;
                            if (mReferences.remove(wrapper)) {
                                try {
                                    wrapper.cleanup();
                                } catch (Exception e) {
                                    // Nothing we can do, just keep going
                                    System.out.println("Exception in native cleanup: " + e.getCause());
                                }
                            }
                        }
                    }
                    catch (InterruptedException e) {
                        // Leaving the thread
                    }
                }
            };
        // Set the priority slightly below NORM_PRIORITY
        // NORM_PRIORITY - 1 maps to ANDROID_PRIORITY_BACKGROUND (nice value 10)
        // https://android.googlesource.com/platform/dalvik/+/eclair-release/vm/Thread.c#3057
        mThread.setPriority(Thread.NORM_PRIORITY - 1);
        mThread.start();
    }

    // public ------------------------------------

    public static void register(Object o, long nativeRef) {
        register(o, o.getClass(), nativeRef);
    }

    public static void register(Object o, Class<?> clazz, long nativeRef) {
        try {
            NativeObjectWrapper wrapper = new NativeObjectWrapper(o, clazz, nativeRef, Holder.instance.mReferenceQueue);
            Holder.instance.mReferences.put(wrapper, true);
        } catch (NoSuchMethodException e) {
            throw new RuntimeException("failed to register object of type " + o.getClass().getName() + " no static method nativeDestroy() found");
        }
    }

    public static void stop() {
        Holder.instance.mThread.interrupt();
    }
}

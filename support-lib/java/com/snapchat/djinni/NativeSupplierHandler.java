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

import com.google.common.base.Supplier;

/**
 * Internal helper class used to bridge native supplier functions to Guava's Supplier interface.
 * This class should not be used directly by application code.
 *
 * Note: This class is intentionally package-private and should only be instantiated
 * by the JNI layer through SupplierAdaptor::fromCpp.
 */
final class NativeSupplierHandler<T> implements Supplier<T> {
    private long mNativeRef;

    NativeSupplierHandler(long nativeRef) {
        this.mNativeRef = nativeRef;
    }

    @Override
    public T get() {
        if (mNativeRef == 0) {
            throw new IllegalStateException("NativeSupplierHandler has been destroyed");
        }

        // Call into native code which will invoke the C++ std::function
        // and return the result
        @SuppressWarnings("unchecked")
        T result = (T) nativeGet(mNativeRef);
        return result;
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (mNativeRef != 0) {
                nativeDestroy(mNativeRef);
                mNativeRef = 0;
            }
        } finally {
            super.finalize();
        }
    }

    // These native methods are implemented in Supplier_jni.cpp
    private static native Object nativeGet(long nativeRef);
    private static native void nativeDestroy(long nativeRef);
}

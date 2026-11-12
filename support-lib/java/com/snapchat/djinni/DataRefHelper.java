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

public final class DataRefHelper {
    /** Lets PreferNativeMemory DataRefs use up to limitBytes of malloc memory; 0 (default) is off. */
    public static void setNativeAllocationLimit(long limitBytes) {
        if (limitBytes < 0) {
            throw new IllegalArgumentException("limitBytes must not be negative: " + limitBytes);
        }
        nativeSetNativeAllocationLimit(limitBytes);
    }

    /** Returns the bytes currently held against the native allocation budget. */
    public static long getNativeAllocationBytes() {
        return nativeGetNativeAllocationBytes();
    }

    public static native void nativeDestroy(long nativeRef);

    private static native void nativeSetNativeAllocationLimit(long limitBytes);

    private static native long nativeGetNativeAllocationBytes();
}

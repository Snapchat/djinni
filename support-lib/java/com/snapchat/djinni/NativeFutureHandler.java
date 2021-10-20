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

public class NativeFutureHandler implements FutureHandler {
    private final long mNativeFunc;
    private final long mNativePromise;
    
    public NativeFutureHandler(long nativeFunc, long nativePromise) {
        mNativeFunc = nativeFunc;
        mNativePromise = nativePromise;
    }

    @Override
    public void handleResult(Object res) {
        System.out.println("XXXX");
        nativeHandleResult(mNativeFunc, mNativePromise, res);
    }

    private static native void nativeHandleResult(long nativeFunc, long nativePromise, Object res);
}

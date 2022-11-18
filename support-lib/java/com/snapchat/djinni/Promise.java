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

import java.util.concurrent.atomic.AtomicReference;

public class Promise<T> {

    private AtomicReference<SharedState<T>> _sharedState;
    private final SharedState<T> _sharedStateReadOnly;

    public Promise() {
        _sharedStateReadOnly = new SharedState<T>();
        _sharedState = new AtomicReference<>(_sharedStateReadOnly);
    }

    // Get a future object associated with this promise
    public Future<T> getFuture() {
        return new Future<T>(_sharedStateReadOnly);
    }

    // `setValue()` or `setException()` can only be called once on a
    // promise. After which the internal state becomes invalid.
    public void setValue(T val) {
        SharedState<T> sharedState = _sharedState.getAndSet(null);
        SharedState.Continuation<T> handler = null;
        synchronized(sharedState) {
            sharedState.value = val;
            sharedState.ready = true;
            if (sharedState.handler != null) {
                handler = sharedState.handler;
            } else {
                sharedState.notifyAll();
            }
        }
        if (handler != null) {
            handler.handleResult(sharedState);
        }
    }

    // for Void futures
    public void setValue() {
        setValue(null);
    }

    public void setException(Throwable ex) {
        SharedState<T> sharedState = _sharedState.getAndSet(null);
        SharedState.Continuation<T> handler = null;
        synchronized(sharedState) {
            sharedState.exception = ex;
            if (sharedState.handler != null) {
                handler = sharedState.handler;
            } else {
                sharedState.notifyAll();
            }
        }
        if (handler != null) {
            handler.handleResult(sharedState);
        }
    }
}

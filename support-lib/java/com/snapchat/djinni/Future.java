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

public class Future<T> {
    // Handler routine for type U that does not return a value
    public interface FutureHandler<U> {
        public void handleResult(Future<U> res) throws Throwable;
    }
    // Handler routine for type U that returns a value of type R
    public interface FutureHandlerWithReturn<U, R> {
        public R handleResult(Future<U> res) throws Throwable;
    }
    
    private AtomicReference<SharedState<T>> _sharedState;

    Future(SharedState<T> state) {
        _sharedState = new AtomicReference(state);
    }
    // If the future is ready, then calling its `get()` method will not block.
    public boolean isReady() {
        SharedState<T> sharedState = _sharedState.get();
        synchronized(sharedState) {
            return sharedState.isReady();
        }
    }
    // Wait until future becomes ready
    public void wait() {
        SharedState<T> sharedState = _sharedState.get();
        synchronized(sharedState) {
            try {
                while(!sharedState.isReady()) {
                    sharedState.wait();
                }
            } catch (InterruptedException e) {
            }
        }
    }
    // Block and wait for the result (or exception). This can only be called
    // once.
    public T get() throws Throwable {
        SharedState<T> sharedState = _sharedState.getAndSet(null);
        synchronized(sharedState) {
            try {
                while(!sharedState.isReady()) {
                    sharedState.wait();
                }
                if (sharedState.exception == null) {
                    return sharedState.value;
                } else {
                    throw sharedState.exception;
                }
            } catch (InterruptedException e) {
                return null;
            }
        }
    }
    // Tell the future to Call the specified handler routine when it becomes
    // ready. Returns a new void future. The current future becomes invalid
    // after this call.
    public Future<Void> then (FutureHandler<T> handler) {
        final Promise<Void> nextPromise = new Promise<Void>();
        final Future<Void> nextFuture = nextPromise.getFuture();
        final SharedState.Continuation<T> continuation = (SharedState<T> res) -> {
            try {
                handler.handleResult(new Future<T>(res));
                nextPromise.setValue(null);
            } catch (Throwable e) {
                nextPromise.setException(e);
            }
        };
        SharedState<T> sharedState = _sharedState.getAndSet(null);
        SharedState<T> sharedStateForReadyFuture = null;
        synchronized(sharedState) {
            if (sharedState.isReady()) {
                sharedStateForReadyFuture = sharedState;
            } else {
                sharedState.handler = continuation;
            }
        }
        if (sharedStateForReadyFuture != null) {
            continuation.handleResult(sharedStateForReadyFuture);
        }
        return nextFuture;
    }
    // Tell the future to Call the specified handler routine when it becomes
    // ready. Returns a new future that wraps the return value of the handler
    // routine. The current future becomes invalid after this call.
    public <R> Future<R> then (final FutureHandlerWithReturn<T, R> handler) {
        final Promise<R> nextPromise = new Promise<R>();
        final Future<R> nextFuture = nextPromise.getFuture();
        final SharedState.Continuation<T> continuation = (SharedState<T> res) -> {
            try {
                nextPromise.setValue(handler.handleResult(new Future<T>(res)));
            } catch (Throwable e) {
                nextPromise.setException(e);
            }
        };
        SharedState<T> sharedState = _sharedState.getAndSet(null);
        SharedState<T> sharedStateForReadyFuture = null;
        synchronized(sharedState) {
            if (sharedState.isReady()) {
                sharedStateForReadyFuture = sharedState;
            } else {
                sharedState.handler = continuation;
            }
        }
        if (sharedStateForReadyFuture != null) {
            continuation.handleResult(sharedStateForReadyFuture);
        }
        return nextFuture;
    }
}

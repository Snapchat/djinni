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

public class Future<T> {

    public interface FutureHandler<U> {
        public void handleResult(Future<U> res) throws Throwable;
    }

    public interface FutureHandlerWithReturn<U, R> {
        public R handleResult(Future<U> res) throws Throwable;
    }
    
    private SharedState<T> sharedState;

    Future(SharedState<T> state) {
        sharedState = state;
    }

    public boolean isReady() {
        synchronized(sharedState) {
            return sharedState.isReady();
        }
    }

    public T get() throws Throwable {
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

        synchronized(sharedState) {
            SharedState<T> st = sharedState;
            sharedState = null;
            if (st.isReady()) {
                continuation.handleResult(st);
            } else {
                st.handler = continuation;
            }
        }

        return nextFuture;
    }

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

        synchronized(sharedState) {
            SharedState<T> st = sharedState;
            sharedState = null;
            if (st.isReady()) {
                continuation.handleResult(st);
            } else {
                st.handler = continuation;
            }
        }
        return nextFuture;
    }
}
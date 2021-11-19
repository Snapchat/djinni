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

#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <condition_variable>
#include <mutex>

namespace djinni {

template <typename T>
struct ValueHolder {
    using type = T;
    std::optional<T> value;
    T getValueUnsafe() const {return *value;}
};
template <>
struct ValueHolder<void> {
    using type = bool;
    std::optional<bool> value;
    void getValueUnsafe() const {}
};

template<typename T>
struct SharedState: ValueHolder<T> {
    std::condition_variable cv;
    std::mutex mutex;
    std::function<void(std::shared_ptr<SharedState>)> handler;
};

template<typename T>
using SharedStatePtr = std::shared_ptr<SharedState<T>>;

template <typename T>
class Future;

template <typename T>
class PromiseBase {
public:
    Future<T> getFuture();

protected:
    void setValue(typename ValueHolder<T>::type val) {
        decltype(_sharedState->handler) handler;
        {
            std::unique_lock lk(_sharedState->mutex);
            _sharedState->value = val;
            handler = std::move(_sharedState->handler);
        }
        if (handler) {
            // handler already assigned
            handler(_sharedState);
        } else {
            _sharedState->cv.notify_all();
        }
    }
private:
    SharedStatePtr<T> _sharedState = std::make_shared<SharedState<T>>();
};

template <typename T>
class Promise: public PromiseBase<T> {
public:
    using PromiseBase<T>::setValue;
};

template <>
class Promise<void>: public PromiseBase<void> {
public:
    void setValue() {
        PromiseBase<void>::setValue(true);
    }
};

template<typename T>
class Future {
    template<typename U>
    friend class PromiseBase;
    
    Future(SharedStatePtr<T> sharedState) : _sharedState(sharedState) {}
public:
    Future(Future&& other) = default;

    bool isValid() const {
        return _sharedState != nullptr;
    }
    
    bool isReady() const {
        std::unique_lock lk(_sharedState->mutex);
        return _sharedState->value.has_value();
    }
    
    void wait() const {
        std::unique_lock lk(_sharedState->mutex);
        _sharedState->cv.wait(lk, [state = _sharedState] { return state->value.has_value(); });
    }

    auto get() const {
        std::unique_lock lk(_sharedState->mutex);
        _sharedState->cv.wait(lk, [state = _sharedState] { return state->value.has_value(); });
        return _sharedState->getValueUnsafe();
    }

    template<typename FUNC>
    auto then(FUNC handler) {
        using HandlerReturnType = std::invoke_result_t<FUNC, Future<T>>;

        std::unique_lock lk(_sharedState->mutex);
        auto sharedState = std::move(_sharedState);

        if constexpr(std::is_void_v<HandlerReturnType>) {
            Promise<void> nextPromise;
            auto nextFuture = nextPromise.getFuture();
            auto continuation = [this, handler, nextPromise] (SharedStatePtr<T> x) mutable {
                handler(Future<T>(x));
                nextPromise.setValue();
            };
            if (sharedState->value.has_value()) {
                // result already available
                continuation(sharedState);
            } else {
                // result not yet available
                sharedState->handler = std::move(continuation);
            }
            return nextFuture;
        } else {
            Promise<HandlerReturnType> nextPromise;
            auto nextFuture = nextPromise.getFuture();
            auto continuation = [this, handler, nextPromise] (SharedStatePtr<T> x) mutable {
                nextPromise.setValue(handler(Future<T>(x)));
            };
            if (sharedState->value.has_value()) {
                // result already available
                continuation(sharedState);
            } else {
                // result not yet available
                sharedState->handler = std::move(continuation);
            }
            return nextFuture;
        }
    }
private:
    SharedStatePtr<T> _sharedState;
};

template <typename T>
Future<T> PromiseBase<T>::getFuture() {
    return Future<T>(_sharedState);
}

} // namespace djinni

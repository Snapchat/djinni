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
};
template <>
struct ValueHolder<void> {
    using type = bool;
};

template<typename T>
struct SharedState {
    std::optional<typename ValueHolder<T>::type> value;
    std::condition_variable cv;
    std::mutex mutex;
    std::function<void(typename ValueHolder<T>::type)> handler;
};

template<typename T>
using SharedStatePtr = std::shared_ptr<SharedState<T>>;

template <typename T>
class Future;

template <typename T>
class Promise {
public:
    Future<T> getFuture();

    void setValue(typename ValueHolder<T>::type val) {
        {
            std::unique_lock lk(_sharedState->mutex);
            _sharedState->value = val;
            if (_sharedState->handler) {
                // handler already assigned
                // TODO call handler without lock
                _sharedState->handler(val);
            }
        }
        _sharedState->cv.notify_all();
    }
private:
    SharedStatePtr<T> _sharedState = std::make_shared<SharedState<T>>();
};

template<typename T>
class Future {
    template<typename U>
    friend class Promise;
    
    Future(SharedStatePtr<T> sharedState) : _sharedState(sharedState) {}
public:

    bool isValid() const {
        return _sharedState != nullptr;
    }
    
    bool isReady() const {
        std::unique_lock lk(_sharedState->mutex);
        return _sharedState->value.has_value();
    }
    
    T get() const {
        std::unique_lock lk(_sharedState->mutex);
        _sharedState->cv.wait(lk, [state = _sharedState] { return state->value.has_value(); });
        return *(_sharedState->value);
    }

    template<typename FUNC>
    auto then(FUNC handler) const {
        // TODO can only call once
        std::unique_lock lk(_sharedState->mutex);

        using HandlerReturnType = std::invoke_result_t<FUNC, T>;

        if constexpr(std::is_void_v<HandlerReturnType>) {
            Promise<void> nextPromise;
            auto nextFuture = nextPromise.getFuture();
            auto continuation = [handler, p = std::move(nextPromise)] (T x) mutable {
                handler(x);
                p.setValue(true);
            };
            if (_sharedState->value.has_value()) {
                // result already available
                continuation(*(_sharedState->value));
            } else {
                // result not yet available
                _sharedState->handler = continuation;
            }
            return nextFuture;
        } else {
            Promise<HandlerReturnType> nextPromise;
            auto nextFuture = nextPromise.getFuture();
            auto continuation = [handler, p = std::move(nextPromise)] (T x) mutable {
                p.setValue(handler(x));
            };
            if (_sharedState->value.has_value()) {
                // result already available
                continuation(*(_sharedState->value));
            } else {
                // result not yet available
                _sharedState->handler = continuation;
            }
            return nextFuture;
        }
    }
private:
    mutable SharedStatePtr<T> _sharedState;
};

template <typename T>
Future<T> Promise<T>::getFuture() {
    // TODO can only call once
    return Future<T>(_sharedState);
}

} // namespace djinni

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

template<typename T>
struct SharedState {
    std::optional<T> value;
    std::condition_variable cv;
    std::mutex mutex;
    std::function<void(T)> handler;
};

template<typename T>
using SharedStatePtr = std::shared_ptr<SharedState<T>>;

template<typename T>
class Future {
    template<typename U>
    friend class Promise;
    
    Future(SharedStatePtr<T> sharedState) : _sharedState(sharedState) {}
public:
    bool isReady() const {
        std::unique_lock lk(_sharedState->mutex);
        return _sharedState->value.has_value();
    }
    
    T get() {
        std::unique_lock lk(_sharedState->mutex);
        _sharedState->cv.wait(lk, [state = _sharedState] { return state->value.has_value(); });
        return *(_sharedState->value);
    }

    void then(std::function<void(T)> handler) const {
        std::unique_lock lk(_sharedState->mutex);
        if (_sharedState->value.has_value()) {
            handler(*(_sharedState->value));
        } else {
            _sharedState->handler = handler;
        }
    }
private:
    mutable SharedStatePtr<T> _sharedState;
};

template <typename T>
class Promise {
public:
    Future<T> getFuture() {
        return Future<T>(_sharedState);
    }
    void setValue(T val) {
        {
            std::unique_lock lk(_sharedState->mutex);
            if (_sharedState->handler) {
                _sharedState->handler(val);
            } else {
                _sharedState->value = val;
            }
        }
        _sharedState->cv.notify_all();
    }
private:
    SharedStatePtr<T> _sharedState = std::make_shared<SharedState<T>>();
};

} // namespace djinni

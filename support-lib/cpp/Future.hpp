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

namespace snapchat::djinni {
namespace detail {

// A wrapper object to support both void and non-void result types in
// Promise/Future
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
struct SharedState;

// A simple type erased function container. It would be nice if std::function<>
// supports move only lambdas.
template <typename T>
struct ValueHandlerBase {
    virtual ~ValueHandlerBase() = default;
    virtual void call(const std::shared_ptr<SharedState<T>>&) = 0;
};
template <typename T, typename F>
class ValueHandler : public ValueHandlerBase<T> {
public:
    ValueHandler(F&& f) : _f(std::move(f)) {}
    ValueHandler(ValueHandler&& other) : _f(std::move(other._f)) {}
    void call (const std::shared_ptr<SharedState<T>>& s) override {
        _f(s);
    }
    
private:
    std::decay_t<F> _f;
};
template<typename T, typename FUNC>
static std::unique_ptr<ValueHandler<T, FUNC>> createValueHandler(FUNC&& f) {
    return std::make_unique<ValueHandler<T, FUNC>>(std::forward<FUNC>(f));
}

// The shared state object that links the promise and future objects
template<typename T>
struct SharedState: ValueHolder<T> {
    std::condition_variable cv;
    std::mutex mutex;
    std::exception_ptr exception;
    std::unique_ptr<ValueHandlerBase<T>> handler;

    bool isReady() const {
        return this->value.has_value() || exception != nullptr;
    }
};

template<typename T>
using SharedStatePtr = std::shared_ptr<SharedState<T>>;
}

template <typename T>
class Future;

namespace detail {
// Common promise base class, shared by both `void` and `T` results.
template <typename T>
class PromiseBase {
public:
    Future<T> getFuture();

protected:
    // setValue can only be called once. After which the shared state is set to
    // null and further calls to setValue will fail.
    void setValue(typename ValueHolder<T>::type val) {
        detail::SharedStatePtr<T> sharedState;
        {
            std::unique_lock lk(_sharedState->mutex);
            _sharedState->value = val;
            sharedState = std::move(_sharedState);
        }
        callResultHandler(sharedState);
    }
    // setException can only be called once
    template <typename E>
    void setException(const E& ex) {
        setException(std::make_exception_ptr(ex));
    }
    void setException(std::exception_ptr ex) {
        detail::SharedStatePtr<T> sharedState;
        {
            std::unique_lock lk(_sharedState->mutex);
            _sharedState->exception = ex;
            sharedState = std::move(_sharedState);
        }
        callResultHandler(sharedState);
    }
private:
    detail::SharedStatePtr<T> _sharedState = std::make_shared<SharedState<T>>();

    void callResultHandler(detail::SharedStatePtr<T> sharedState) {
        if (sharedState->handler) {
            // handler already assigned
            sharedState->handler->call(sharedState);
        } else {
            sharedState->cv.notify_all();
        }
    }
};
}

// Promise with non-void result type `T`
template <typename T>
class Promise: public detail::PromiseBase<T> {
public:
    using detail::PromiseBase<T>::setValue;
    using detail::PromiseBase<T>::setException;
    // default constructable
    Promise() = default;
    // moveable
    Promise(Promise&&) noexcept = default;
    Promise& operator= (Promise&&) noexcept = default;
    // not copyable
    Promise(const Promise&) = delete;
    Promise& operator= (const Promise&) noexcept = delete;
};

// Promise with a void result
template <>
class Promise<void>: public detail::PromiseBase<void> {
public:
    void setValue() {
        detail::PromiseBase<void>::setValue(true);
    }
    using detail::PromiseBase<void>::setException;
    // default constructable
    Promise() = default;
    // moveable
    Promise(Promise&&) noexcept = default;
    Promise& operator= (Promise&&) noexcept = default;
    // not copyable
    Promise(const Promise&) = delete;
    Promise& operator= (const Promise&) noexcept = delete;
};

template<typename T>
class Future {
    template<typename U>
    friend class detail::PromiseBase;
    // not user constructable
    Future(detail::SharedStatePtr<T> sharedState) : _sharedState(sharedState) {}
public:
    // moveable
    Future(Future&& other) noexcept = default;
    Future& operator= (Future&& other) noexcept = default;
    // not copyable
    Future(const Future& other) = delete;
    Future& operator= (const Future& other) noexcept = delete;
    // Future becomes invalid after `then()` is called on it
    bool isValid() const {
        return _sharedState != nullptr;
    }
    // returns true if the result can be `get()` without blocking
    bool isReady() const {
        std::unique_lock lk(_sharedState->mutex);
        return _sharedState->isReady();
    }
    // wait until future becomes `isReady()`
    void wait() const {
        std::unique_lock lk(_sharedState->mutex);
        _sharedState->cv.wait(lk, [state = _sharedState] {return state->isReady();});
    }
    // wait until future becomes `isReady()` and return the result
    auto get() const {
        std::unique_lock lk(_sharedState->mutex);
        _sharedState->cv.wait(lk, [state = _sharedState] {return state->isReady();});
        if (!_sharedState->exception) {
            return _sharedState->getValueUnsafe();
        } else {
            std::rethrow_exception(_sharedState->exception);
        }
    }

    template<typename FUNC>
    auto then(FUNC handler) {
        using HandlerReturnType = std::invoke_result_t<FUNC, Future<T>>;

        detail::SharedStatePtr<T> sharedState;
        {
            std::unique_lock lk(_sharedState->mutex);
            sharedState = std::move(_sharedState);
        }
        auto nextPromise = std::make_unique<Promise<HandlerReturnType>>();
        auto nextFuture = nextPromise->getFuture();
        auto continuation = [this, handler = std::move(handler), nextPromise = std::move(nextPromise)] (detail::SharedStatePtr<T> x) mutable {
            try {
                if constexpr(std::is_void_v<HandlerReturnType>) {
                    handler(Future<T>(x));
                    nextPromise->setValue();
                } else {
                    nextPromise->setValue(handler(Future<T>(x)));
                }
            } catch (std::exception& e) {
                nextPromise->setException(std::current_exception());
            }
        };
        if (sharedState->isReady()) {
            // result already available
            continuation(sharedState);
        } else {
            // result not yet available
            sharedState->handler = detail::createValueHandler<T>(std::move(continuation));
        }
        return nextFuture;
    }
private:
    detail::SharedStatePtr<T> _sharedState;
};

template <typename T>
Future<T> detail::PromiseBase<T>::getFuture() {
    return Future<T>(_sharedState);
}

} // namespace snapchat::djinni

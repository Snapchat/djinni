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
#include <cassert>

#if defined(DJINNI_FUTURE_COROUTINE_SUPPORT)
#include <experimental/coroutine>
#endif

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
    ValueHandler(F&& f) : _f(std::forward<F>(f)) {}
    ValueHandler(ValueHandler&& other) : _f(std::move(other._f)) {}
    void call (const std::shared_ptr<SharedState<T>>& s) override {
        _f(s);
    }
private:
    std::decay_t<F> _f;
};
template<typename T, typename FUNC>
static auto createValueHandler(FUNC&& f) {
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
    // `setValue()` or `setException()` can only be called once. After which the
    // shared state is set to null and further calls to `setValue()` or
    // `setException()` will fail.  If at the moment of calling `setValue()` or
    // `setException()`, the `then()` method is already called on the future
    // object, then the handler routine specified by `then()` will immediately
    // be called in the current thread.
    void setValue(const typename ValueHolder<T>::type& val) {
        updateAndCallResultHandler([&] (const detail::SharedStatePtr<T>& sharedState) {
            sharedState->value = val;
        });
    }
    void setValue(typename ValueHolder<T>::type&& val) {
        updateAndCallResultHandler([&] (const detail::SharedStatePtr<T>& sharedState) {
            sharedState->value = std::move(val);
        });
    }
    template <typename E>
    void setException(const E& ex) {
        setException(std::make_exception_ptr(ex));
    }
    void setException(std::exception_ptr ex) {
        updateAndCallResultHandler([&] (const detail::SharedStatePtr<T>& sharedState) {
            sharedState->exception = ex;
        });
    }
private:
    detail::SharedStatePtr<T> _sharedState = std::make_shared<SharedState<T>>();

    template <typename UpdateFunc>
    void updateAndCallResultHandler(UpdateFunc&& updater) {
        detail::SharedStatePtr<T> sharedState;
        sharedState = std::atomic_exchange(&_sharedState, sharedState);
        assert(sharedState);    // a second call will trigger assertion
        std::unique_ptr<ValueHandlerBase<T>> handler;
        {
            std::lock_guard lk(sharedState->mutex);
            updater(sharedState);
            handler = std::move(sharedState->handler);
        }
        if (handler) {
            // handler already assigned, call it inline
            handler->call(sharedState);
        } else {
            // otherwise unblock potential waiters
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
    Promise& operator= (const Promise&) = delete;
};

// Promise with a void result
template <>
class Promise<void>: public detail::PromiseBase<void> {
public:
    void setValue() {setValue(true);}
    using detail::PromiseBase<void>::setException;
    // default constructable
    Promise() = default;
    // moveable
    Promise(Promise&&) noexcept = default;
    Promise& operator= (Promise&&) noexcept = default;
    // not copyable
    Promise(const Promise&) = delete;
    Promise& operator= (const Promise&) = delete;
private:
    // hide the bool version
    void setValue(const bool&) {detail::PromiseBase<void>::setValue(true);}
    void setValue(bool&&) {detail::PromiseBase<void>::setValue(true);}
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
    Future& operator= (const Future& other) = delete;
    // Future becomes invalid after `then()` is called on it
    bool isValid() const {
        return std::atomic_load(&_sharedState) != nullptr;
    }
    // returns true if the result can be `get()` without blocking
    bool isReady() const {
        auto sharedState = std::atomic_load(&_sharedState);
        assert(sharedState);    // call on invalid future will trigger assertion
        std::unique_lock lk(sharedState->mutex);
        return sharedState->isReady();
    }
    // wait until future becomes `isReady()`
    void wait() const {
        auto sharedState = std::atomic_load(&_sharedState);
        assert(sharedState);    // call on invalid future will trigger assertion
        std::unique_lock lk(sharedState->mutex);
        sharedState->cv.wait(lk, [state = sharedState] {return state->isReady();});
    }
    // wait until future becomes `isReady()` and return the result. This can
    // only be called once.
    auto get() {
        detail::SharedStatePtr<T> sharedState;
        sharedState = std::atomic_exchange(&_sharedState, sharedState);
        assert(sharedState);    // call on invalid future will trigger assertion
        std::unique_lock lk(sharedState->mutex);
        sharedState->cv.wait(lk, [state = sharedState] {return state->isReady();});
        if (!sharedState->exception) {
            return sharedState->getValueUnsafe();
        } else {
            std::rethrow_exception(sharedState->exception);
        }
    }
    // If at the moment of calling `then()`, the result (or exception) is
    // already available, then the handler routine will immediately be called in
    // the current thread. Returns a new future that wraps the return value of
    // the handler routine. The current future becomes invalid after this call.
    template<typename FUNC>
    auto then(FUNC&& handler) {
        using HandlerReturnType = std::invoke_result_t<FUNC, Future<T>>;
        detail::SharedStatePtr<T> sharedState;
        sharedState = std::atomic_exchange(&_sharedState, sharedState);
        assert(sharedState);    // a second call will trigger assertion
        auto nextPromise = std::make_unique<Promise<HandlerReturnType>>();
        auto nextFuture = nextPromise->getFuture();
        auto continuation = [handler = std::forward<FUNC>(handler), nextPromise = std::move(nextPromise)] (detail::SharedStatePtr<T> x) mutable {
            try {
                if constexpr(std::is_void_v<HandlerReturnType>) {
                    handler(Future<T>(x));
                    nextPromise->setValue();
                } else {
                    nextPromise->setValue(handler(Future<T>(x)));
                }
            } catch (const std::exception& e) {
                nextPromise->setException(std::current_exception());
            }
        };
        detail::SharedStatePtr<T> sharedStateForReadyFuture;
        {
            std::unique_lock lk(sharedState->mutex);
            if (sharedState->isReady()) {
                // result already available
                sharedStateForReadyFuture = std::move(sharedState);
            } else {
                // result not yet available
                sharedState->handler = detail::createValueHandler<T>(std::move(continuation));
            }
        }
        if (sharedStateForReadyFuture) {
            continuation(sharedStateForReadyFuture);
        }
        return nextFuture;
    }

private:
    detail::SharedStatePtr<T> _sharedState;

#if defined(DJINNI_FUTURE_COROUTINE_SUPPORT)
public:
    bool await_ready() {
        return isReady();
    }
    auto await_resume() {
        auto sharedState = std::atomic_load(&_sharedState);
        if (sharedState) {
            return Future<T>(sharedState).get();
        } else {
            return Future<T>(_coroState).get();
        }
    }
    bool await_suspend(std::experimental::coroutine_handle<> h) {
        this->then([h, this] (Future<T> x) mutable {
            _coroState = x._sharedState;
            h();
        });
        return true;
    }
private:
    detail::SharedStatePtr<T> _coroState;
#endif
};

template <typename T>
Future<T> detail::PromiseBase<T>::getFuture() {
    return Future<T>(std::atomic_load(&_sharedState));
}

} // namespace snapchat::djinni

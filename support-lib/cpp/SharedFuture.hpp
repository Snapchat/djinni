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

#include "Future.hpp"

#include <memory>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

namespace djinni {

// SharedFuture is a wrapper around djinni::Future to allow multiple consumers (i.e. like std::shared_future)
// The API is designed to be similar to djinni::Future.
template<typename T>
class SharedFuture {
public:
    // Create SharedFuture from Future. Runtime error if the future is already consumed.
    explicit SharedFuture(Future<T>&& future);

    // Transform into Future<T>.
    Future<T> toFuture() const {
        if (await_ready()) {
            co_return await_resume(); // return stored value directly
        }
        co_return co_await SharedFuture(*this); // retain copy during coroutine suspension
    }

    void wait() const {
        return [this]() -> Future<void> { co_await *this; }().wait();
    }

    decltype(auto) get() const {
        wait();
        return await_resume();
    }

    // Transform the result of this future into a new future. The behavior is same as Future::then except that
    // it doesn't consume the future, and can be called multiple times.
    template<typename Func>
    SharedFuture<std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<Func, T>>>> then(Func transform) const {
        co_return transform(co_await SharedFuture(*this)); // retain copy during coroutine suspension
    }

    // Overload for T = void or `transform` takes no arugment.
    template<typename Func, typename = std::enable_if_t<!std::is_invocable_v<Func, T>>>
    SharedFuture<std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<Func>>>> then(Func transform) const {
        co_await SharedFuture(*this); // retain copy during coroutine suspension
        co_return transform();
    }

    // -- coroutine support implementation only; not intended externally --

    bool await_ready() const {
        std::scoped_lock lock(_sharedStates->mutex);
        return _sharedStates->storedValue.has_value();
    }

    decltype(auto) await_resume() const {
        if constexpr (!std::is_void_v<T>) {
            return *_sharedStates->storedValue;
        }
    }

    bool await_suspend(detail::CoroutineHandle<> h) const;

    struct Promise : public Future<T>::promise_type {
        SharedFuture<T> get_return_object() noexcept {
            return SharedFuture(Future<T>::promise_type::get_return_object());
        }
    };
    using promise_type = Promise;

private:
    struct SharedStates {
        std::recursive_mutex mutex;
        std::optional<std::conditional_t<std::is_void_v<T>, std::monostate, T>> storedValue = std::nullopt;
        std::vector<detail::CoroutineHandle<>> coroutineHandles;
    };
    // Use a shared_ptr to allow copying SharedFuture.
    std::shared_ptr<SharedStates> _sharedStates = std::make_shared<SharedStates>();
};

// CTAD deduction guide to construct from Future directly.
template<typename T>
SharedFuture(Future<T>&&) -> SharedFuture<T>;

// ------------------ Implementation ------------------

template<typename T>
SharedFuture<T>::SharedFuture(Future<T>&& future) {
    // `future` will invoke all continuations when it is ready.
    future.then([sharedStates = _sharedStates](auto futureResult) {
        std::vector toCall = [&] {
            std::scoped_lock lock(sharedStates->mutex);
            if constexpr (std::is_void_v<T>) {
                sharedStates->storedValue.emplace();
            } else {
                sharedStates->storedValue = futureResult.get();
            }
            return std::move(sharedStates->coroutineHandles);
        }();
        for (auto& handle : toCall) {
            handle();
        }
    });
}

template<typename T>
bool SharedFuture<T>::await_suspend(detail::CoroutineHandle<> h) const {
    {
        std::unique_lock lock(_sharedStates->mutex);
        if (!_sharedStates->storedValue) {
            _sharedStates->coroutineHandles.push_back(std::move(h));
            return true;
        }
    }
    h();
    return true;
}

} // namespace djinni

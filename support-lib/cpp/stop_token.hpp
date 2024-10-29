#pragma once

#ifdef __has_include
#if __has_include(<version>)
#include <version>
#endif
#endif

// At the time of writing, P2300R10 only defines __cpp_lib_senders as a feature-test, nothing specific to whether inplace_stop_token is available
#ifndef __cpp_lib_senders
#ifndef DJINNI_STOP_SOURCE_USE_CUSTOM_IMPL
#define DJINNI_STOP_SOURCE_USE_CUSTOM_IMPL
#endif
#endif

#ifndef DJINNI_STOP_SOURCE_USE_CUSTOM_IMPL

// Standard implementation is available

#include <stop_token>

namespace djinni {

using inplace_stop_token = std::inplace_stop_token;
using inplace_stop_source = std::inplace_stop_source;
template<typename Callback>
using inplace_stop_callback = std::inplace_stop_callback<Callback>;

}

#else

// Standard implementation unavailable, we roll our own implementation of what we need

#include <memory>
#include <functional>
#include <thread>

namespace djinni {

namespace details {

struct stop_callback_node final {
    void(*callback)(void*);
    void* arg;
    stop_callback_node* next;
};

};

class inplace_stop_source;
template<typename Callback>
class inplace_stop_callback;

class inplace_stop_token {
public:
    template<class CallbackFn>
    using callback_type = inplace_stop_callback<CallbackFn>;

    inplace_stop_token() = default;
    bool operator==(const inplace_stop_token&) const = default;

    [[nodiscard]] bool stop_requested() const noexcept;
    [[nodiscard]] bool stop_possible() const noexcept;
    void swap(inplace_stop_token&) noexcept;

private:
    template<typename T>
    friend class inplace_stop_callback;
    friend inplace_stop_source;
    constexpr inplace_stop_token(const inplace_stop_source* stop_source) noexcept
    :_stop_source{stop_source} {}

    const inplace_stop_source* _stop_source = nullptr;
};

class inplace_stop_source {
public:
    constexpr inplace_stop_source() noexcept = default;

    inplace_stop_source(inplace_stop_source&&) = delete;
    inplace_stop_source(const inplace_stop_source&) = delete;
    inplace_stop_source& operator=(inplace_stop_source&&) = delete;
    inplace_stop_source& operator=(const inplace_stop_source&) = delete;

    [[nodiscard]] constexpr inplace_stop_token get_token() const noexcept {
        return {this};
    }
    [[nodiscard]] static constexpr bool stop_possible() noexcept { return true; }

    [[nodiscard]] bool stop_requested() const noexcept;
    bool request_stop() noexcept;

private:
    template<typename T>
    friend class inplace_stop_callback;
    void _register_callback(details::stop_callback_node* callback) const noexcept;
    void _unregister_callback(details::stop_callback_node* callback) const noexcept;

    mutable std::mutex _mtx{};
    std::thread::id _stopping_thread{};
    mutable details::stop_callback_node* _cb_node = nullptr;
};

template<typename Callback>
class inplace_stop_callback {
public:
    using callback_type = Callback;

    template<typename C>
    explicit inplace_stop_callback(inplace_stop_token token, C&& callback) noexcept(std::is_nothrow_constructible_v<Callback, C>)
        :_token(std::move(token))
        ,_callback(std::forward<C>(callback))
    {
        if (_token._stop_source) {
            _token._stop_source->_register_callback(&_node);
        }
    }

    ~inplace_stop_callback() {
        if (_token._stop_source) {
            _token._stop_source->_unregister_callback(&_node);
        }
    }

    inplace_stop_callback(const inplace_stop_callback&) = delete;
    inplace_stop_callback& operator=(const inplace_stop_callback&) = delete;
    inplace_stop_callback(inplace_stop_callback&&) = delete;
    inplace_stop_callback& operator=(inplace_stop_callback&&) = delete;

private:
    static void invoke(void* callback) {
        reinterpret_cast<inplace_stop_callback*>(callback)->_callback();
    }

    inplace_stop_token _token;
    Callback _callback;
    details::stop_callback_node _node {
        .callback = &inplace_stop_callback<Callback>::invoke,
        .arg = this,
        .next = nullptr,
    };
};

template<typename C>
inplace_stop_callback(inplace_stop_token, C callback) -> inplace_stop_callback<C>;

/*
Ignored the following std requirements for simplicity and C++17 compatibility:
- template arg of inplace_stop_callback must be std::invocable and std::destructible
*/

}

#endif

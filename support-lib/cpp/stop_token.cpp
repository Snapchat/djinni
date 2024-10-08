#include "stop_token.hpp"

#ifdef DJINNI_STOP_SOURCE_USE_CUSTOM_IMPL

namespace djinni {

bool inplace_stop_token::stop_requested() const noexcept {
    return stop_possible() && _stop_source->stop_requested();
}

bool inplace_stop_token::stop_possible() const noexcept {
    return _stop_source != nullptr;
}

void inplace_stop_token::swap(inplace_stop_token& other) noexcept {
    std::swap(other._stop_source, _stop_source);
}


bool inplace_stop_source::stop_requested() const noexcept {
    return _stopping_thread != std::thread::id{};
}

bool inplace_stop_source::request_stop() noexcept {
    std::unique_lock lock{_mtx};
    if (_stopping_thread != std::thread::id{}) {
        return false;
    }

    _stopping_thread = std::this_thread::get_id();

    for (; _cb_node != nullptr; _cb_node = _cb_node->next) {
        lock.unlock();
        (*_cb_node->callback)(_cb_node->arg);
        lock.lock();
    }
    return true;
}

void inplace_stop_source::_register_callback(details::stop_callback_node* node) const noexcept {
    std::unique_lock lock{_mtx};
    if (_stopping_thread != std::thread::id{}) {
        lock.unlock();
        (*node->callback)(node->arg);
        return;
    }

    node->next = _cb_node;
    _cb_node = node->next;
}

void inplace_stop_source::_unregister_callback(details::stop_callback_node* node) const noexcept {
    std::unique_lock lock{_mtx};
    if (_stopping_thread != std::thread::id{} && _cb_node == node) {
        // we're currently being stopped and the given callback is the one that is currently being executed
        if (_stopping_thread == std::this_thread::get_id()) {
            // the current callback is removing itself during its execution, so nothing to do
            return;
        } else {
            // another thread is currently executing the callback, we have to block until it completes
            // so we insert a phony callback that will alert us once the current callback is finished
            bool finished{false};
            std::condition_variable cv{};
            auto finish = [this, &finished, &cv] { 
                {
                    std::lock_guard<std::mutex> lock{_mtx};
                    finished = true;
                }
                cv.notify_one();
            };
            using FinishFunctor = decltype(finish);

            void(*finished_callback)(void*) = [](void* arg) {
                auto& finish{*reinterpret_cast<FinishFunctor*>(arg)};
                finish();
            };


            details::stop_callback_node node {
                .callback = finished_callback,
                .arg = &finish,
                .next = _cb_node->next,
            };
            _cb_node->next = &node;

            // Wait until the phony callback was invoked
            cv.wait(lock, [&finished] {
                return finished;
            });

            return;
        }
    }

    for (auto* prev_node = _cb_node; prev_node->next != nullptr; prev_node = prev_node->next) {
        if (prev_node->next == node) {
            prev_node->next = node->next;
            return;
        }
    }
}

}

#endif

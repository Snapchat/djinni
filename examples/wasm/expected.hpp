#pragma once

#include "cpp/expected.hpp"

namespace djinni {

using ::tl::unexpected;
using ::tl::expected;

template <class E>
unexpected<typename std::decay<E>::type> make_unexpected(E &&e) {
    return tl::unexpected<typename std::decay<E>::type>(std::forward<E>(e));
}

}

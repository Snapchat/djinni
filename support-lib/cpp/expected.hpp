#pragma once

#include <version>

#ifdef __cpp_lib_expected

#include <expected>

namespace djinni {

using ::std::expected;
using ::std::unexpected;

}

#else

#include "tl_expected.hpp"

namespace djinni {

using ::tl::unexpected;
using ::tl::expected;

}

#endif
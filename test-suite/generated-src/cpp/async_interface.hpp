#pragma once

#include "Future.hpp"

namespace testsuite {

class AsyncInterface {
public:
    virtual ~AsyncInterface() {}

    virtual djinni::Future<int32_t> get_async_result() = 0;
};

}  // namespace testsuite

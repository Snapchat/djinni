// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from multiple_inheritance.djinni

#pragma once

#include <cstdint>
#include <memory>

namespace testsuite {

/** Used for C++ multiple inheritance tests */
class ReturnTwo {
public:
    virtual ~ReturnTwo() = default;

    static /*not-null*/ std::shared_ptr<ReturnTwo> get_instance();

    virtual int8_t return_two() = 0;
};

} // namespace testsuite

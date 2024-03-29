// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from outcome.djinni

#pragma once

#include "expected.hpp"
#include <cstdint>
#include <string>

namespace testsuite {

struct NestedOutcome;

class TestOutcome {
public:
    virtual ~TestOutcome() = default;

    static djinni::expected<std::string, int32_t> getSuccessOutcome();

    static djinni::expected<std::string, int32_t> getErrorOutcome();

    static std::string putSuccessOutcome(const djinni::expected<std::string, int32_t> & x);

    static int32_t putErrorOutcome(const djinni::expected<std::string, int32_t> & x);

    static NestedOutcome getNestedSuccessOutcome();

    static NestedOutcome getNestedErrorOutcome();

    static int32_t putNestedSuccessOutcome(const NestedOutcome & x);

    static std::string putNestedErrorOutcome(const NestedOutcome & x);
};

} // namespace testsuite

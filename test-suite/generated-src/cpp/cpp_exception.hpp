// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from exception.djinni

#pragma once

#include <cstdint>
#include <memory>

namespace testsuite {

class CppException {
public:
    virtual ~CppException() = default;

    virtual int32_t throw_an_exception() = 0;

    static /*not-null*/ std::shared_ptr<CppException> get();
};

}  // namespace testsuite

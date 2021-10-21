// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from client_interface.djinni

#pragma once

#include "../../handwritten-src/cpp/optional.hpp"
#include <memory>
#include <string>

namespace testsuite {

class ReverseClientInterface {
public:
    virtual ~ReverseClientInterface() = default;

    virtual std::string return_str() const = 0;

    virtual std::string meth_taking_interface(const std::shared_ptr<ReverseClientInterface> & i) = 0;

    virtual std::string meth_taking_optional_interface(const std::shared_ptr<ReverseClientInterface> & i) = 0;

    static std::shared_ptr<ReverseClientInterface> create();
};

}  // namespace testsuite

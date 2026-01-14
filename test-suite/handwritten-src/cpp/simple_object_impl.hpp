#pragma once

#include "simple_object.hpp"
#include <string>

namespace testsuite {

class SimpleObjectImpl : public SimpleObject {
public:
    SimpleObjectImpl(int32_t value, std::string name) 
        : _value(value), _name(std::move(name)) {}

    int32_t get_value() override { return _value; }
    std::string get_name() override { return _name; }

private:
    int32_t _value;
    std::string _name;
};

} // namespace testsuite

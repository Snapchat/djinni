#include "test_provider.hpp"
#include "nested_provider.hpp"
#include "nested_provider_interface.hpp"
#include "simple_object_impl.hpp"
#include <string>
#include <functional>
#include <memory>
#include <stdexcept>

namespace testsuite {

std::function<std::string()> TestProvider::getProviderString() {
    return []() -> std::string {
        return "hello";
    };
}

std::function<int32_t()> TestProvider::getProviderInt() {
    return []() -> int32_t {
        return 42;
    };
}

std::string TestProvider::callProviderString(const std::function<std::string()>& x) {
    // Verify host provider can be called more than once by C++
    std::string first = x();
    std::string second = x();
    if (first != second) {
        throw std::logic_error("callProviderString: provider returned different values on multiple calls");
    }
    return first;
}

int32_t TestProvider::callProviderInt(const std::function<int32_t()>& x) {
    // Verify host provider can be called more than once by C++
    int32_t first = x();
    int32_t second = x();
    if (first != second) {
        throw std::logic_error("callProviderInt: provider returned different values on multiple calls");
    }
    return first;
}

NestedProvider TestProvider::getNestedProvider() {
    return {
        []() -> int32_t {
            return 99;
        }
    };
}

int32_t TestProvider::callNestedProvider(const NestedProvider& x) {
    // Verify host nested provider can be called more than once by C++
    int32_t first = x.s();
    int32_t second = x.s();
    if (first != second) {
        throw std::logic_error("callNestedProvider: nested provider returned different values on multiple calls");
    }
    return first;
}

// Interface provider tests - main use case for deferred expensive object creation
std::function<std::shared_ptr<SimpleObject>()> TestProvider::getProviderObject() {
    return []() -> std::shared_ptr<SimpleObject> {
        // Simulate expensive object creation
        return std::make_shared<SimpleObjectImpl>(42, "expensive");
    };
}

int32_t TestProvider::callProviderObject(const std::function<std::shared_ptr<SimpleObject>()>& x) {
    // Verify host provider can be called more than once by C++
    auto obj1 = x();
    auto obj2 = x();
    if (obj1->get_value() != obj2->get_value() || obj1->get_name() != obj2->get_name()) {
        throw std::logic_error("callProviderObject: provider returned different objects on multiple calls");
    }
    return obj1->get_value();
}

std::string TestProvider::callProviderObjectGetName(const std::function<std::shared_ptr<SimpleObject>()>& x) {
    // Verify host provider can be called more than once by C++
    auto obj1 = x();
    auto obj2 = x();
    if (obj1->get_name() != obj2->get_name()) {
        throw std::logic_error("callProviderObjectGetName: provider returned different names on multiple calls");
    }
    return obj1->get_name();
}

NestedProviderInterface TestProvider::getNestedProviderInterface() {
    return {
        []() -> std::shared_ptr<SimpleObject> {
            return std::make_shared<SimpleObjectImpl>(777, "nested");
        }
    };
}

int32_t TestProvider::callNestedProviderInterface(const NestedProviderInterface& x) {
    // Verify host nested provider interface can be called more than once by C++
    auto obj1 = x.obj();
    auto obj2 = x.obj();
    if (obj1->get_value() != obj2->get_value()) {
        throw std::logic_error("callNestedProviderInterface: nested provider returned different values on multiple calls");
    }
    return obj1->get_value();
}

std::shared_ptr<SimpleObject> TestProvider::createSimpleObject(int32_t value, const std::string& name) {
    return std::make_shared<SimpleObjectImpl>(value, name);
}

} // namespace testsuite

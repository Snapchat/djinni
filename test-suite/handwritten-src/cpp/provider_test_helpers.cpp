#include "test_provider.hpp"
#include "nested_provider.hpp"
#include "nested_provider_interface.hpp"
#include "simple_object_impl.hpp"
#include <string>
#include <functional>
#include <memory>

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
    return x();
}

int32_t TestProvider::callProviderInt(const std::function<int32_t()>& x) {
    return x();
}

NestedProvider TestProvider::getNestedProvider() {
    return {
        []() -> int32_t {
            return 99;
        }
    };
}

int32_t TestProvider::callNestedProvider(const NestedProvider& x) {
    return x.s();
}

// Interface provider tests - main use case for deferred expensive object creation
std::function<std::shared_ptr<SimpleObject>()> TestProvider::getProviderObject() {
    return []() -> std::shared_ptr<SimpleObject> {
        // Simulate expensive object creation
        return std::make_shared<SimpleObjectImpl>(42, "expensive");
    };
}

int32_t TestProvider::callProviderObject(const std::function<std::shared_ptr<SimpleObject>()>& x) {
    auto obj = x(); // Deferred creation happens here
    return obj->get_value();
}

std::string TestProvider::callProviderObjectGetName(const std::function<std::shared_ptr<SimpleObject>()>& x) {
    auto obj = x();
    return obj->get_name();
}

NestedProviderInterface TestProvider::getNestedProviderInterface() {
    return {
        []() -> std::shared_ptr<SimpleObject> {
            return std::make_shared<SimpleObjectImpl>(777, "nested");
        }
    };
}

int32_t TestProvider::callNestedProviderInterface(const NestedProviderInterface& x) {
    auto obj = x.obj(); // Call the provider function
    return obj->get_value();
}

std::shared_ptr<SimpleObject> TestProvider::createSimpleObject(int32_t value, const std::string& name) {
    return std::make_shared<SimpleObjectImpl>(value, name);
}

} // namespace testsuite

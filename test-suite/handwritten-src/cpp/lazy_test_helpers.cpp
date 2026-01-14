#include "test_lazy.hpp"
#include "nested_lazy.hpp"
#include "nested_lazy_interface.hpp"
#include "simple_object_impl.hpp"
#include <string>
#include <functional>
#include <memory>

namespace testsuite {

std::function<std::string()> TestLazy::getLazyString() {
    return []() -> std::string {
        return "hello";
    };
}

std::function<int32_t()> TestLazy::getLazyInt() {
    return []() -> int32_t {
        return 42;
    };
}

std::string TestLazy::callLazyString(const std::function<std::string()>& x) {
    return x();
}

int32_t TestLazy::callLazyInt(const std::function<int32_t()>& x) {
    return x();
}

NestedLazy TestLazy::getNestedLazy() {
    return {
        []() -> int32_t {
            return 99;
        }
    };
}

int32_t TestLazy::callNestedLazy(const NestedLazy& x) {
    return x.l();
}

// Interface lazy tests - main use case for deferred expensive object creation
std::function<std::shared_ptr<SimpleObject>()> TestLazy::getLazyObject() {
    return []() -> std::shared_ptr<SimpleObject> {
        // Simulate expensive object creation
        return std::make_shared<SimpleObjectImpl>(42, "expensive");
    };
}

int32_t TestLazy::callLazyObject(const std::function<std::shared_ptr<SimpleObject>()>& x) {
    auto obj = x(); // Deferred creation happens here
    return obj->get_value();
}

std::string TestLazy::callLazyObjectGetName(const std::function<std::shared_ptr<SimpleObject>()>& x) {
    auto obj = x();
    return obj->get_name();
}

NestedLazyInterface TestLazy::getNestedLazyInterface() {
    return {
        []() -> std::shared_ptr<SimpleObject> {
            return std::make_shared<SimpleObjectImpl>(777, "nested");
        }
    };
}

int32_t TestLazy::callNestedLazyInterface(const NestedLazyInterface& x) {
    auto obj = x.obj(); // Call the lazy function
    return obj->get_value();
}

std::shared_ptr<SimpleObject> TestLazy::createSimpleObject(int32_t value, const std::string& name) {
    return std::make_shared<SimpleObjectImpl>(value, name);
}

} // namespace testsuite

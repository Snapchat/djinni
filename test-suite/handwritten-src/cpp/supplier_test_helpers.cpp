#include "test_supplier.hpp"
#include "nested_supplier.hpp"
#include "nested_supplier_interface.hpp"
#include "simple_object_impl.hpp"
#include <string>
#include <functional>
#include <memory>

namespace testsuite {

std::function<std::string()> TestSupplier::getSupplierString() {
    return []() -> std::string {
        return "hello";
    };
}

std::function<int32_t()> TestSupplier::getSupplierInt() {
    return []() -> int32_t {
        return 42;
    };
}

std::string TestSupplier::callSupplierString(const std::function<std::string()>& x) {
    return x();
}

int32_t TestSupplier::callSupplierInt(const std::function<int32_t()>& x) {
    return x();
}

NestedSupplier TestSupplier::getNestedSupplier() {
    return {
        []() -> int32_t {
            return 99;
        }
    };
}

int32_t TestSupplier::callNestedSupplier(const NestedSupplier& x) {
    return x.s();
}

// Interface supplier tests - main use case for deferred expensive object creation
std::function<std::shared_ptr<SimpleObject>()> TestSupplier::getSupplierObject() {
    return []() -> std::shared_ptr<SimpleObject> {
        // Simulate expensive object creation
        return std::make_shared<SimpleObjectImpl>(42, "expensive");
    };
}

int32_t TestSupplier::callSupplierObject(const std::function<std::shared_ptr<SimpleObject>()>& x) {
    auto obj = x(); // Deferred creation happens here
    return obj->get_value();
}

std::string TestSupplier::callSupplierObjectGetName(const std::function<std::shared_ptr<SimpleObject>()>& x) {
    auto obj = x();
    return obj->get_name();
}

NestedSupplierInterface TestSupplier::getNestedSupplierInterface() {
    return {
        []() -> std::shared_ptr<SimpleObject> {
            return std::make_shared<SimpleObjectImpl>(777, "nested");
        }
    };
}

int32_t TestSupplier::callNestedSupplierInterface(const NestedSupplierInterface& x) {
    auto obj = x.obj(); // Call the supplier function
    return obj->get_value();
}

std::shared_ptr<SimpleObject> TestSupplier::createSimpleObject(int32_t value, const std::string& name) {
    return std::make_shared<SimpleObjectImpl>(value, name);
}

} // namespace testsuite

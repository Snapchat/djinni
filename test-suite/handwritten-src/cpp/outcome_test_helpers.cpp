#include "test_outcome.hpp"
#include "nested_outcome.hpp"
#include <string>

namespace testsuite {

djinni::expected<std::string, int> TestOutcome::getSuccessOutcome() {
    return std::string("hello");
}

djinni::expected<std::string, int> TestOutcome::getErrorOutcome() {
    return djinni::make_unexpected(42);
}

std::string TestOutcome::putSuccessOutcome(const djinni::expected<std::string, int>& x) {
    return x.value();
}

int32_t TestOutcome::putErrorOutcome(const djinni::expected<std::string, int>& x) {
    return x.error();
}

NestedOutcome TestOutcome::getNestedSuccessOutcome() {
    return {{42}};
}

NestedOutcome TestOutcome::getNestedErrorOutcome() {
    return {
        djinni::make_unexpected(std::string("hello"))
    };
}

int32_t TestOutcome::putNestedSuccessOutcome(const NestedOutcome& x) {
    return x.o.value();
}

std::string TestOutcome::putNestedErrorOutcome(const NestedOutcome& x) {
    return x.o.error();
}

} // namespace testsuite

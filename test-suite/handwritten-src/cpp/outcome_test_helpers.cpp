#include "test_outcome.hpp"
#include "nested_outcome.hpp"
#include "outcome_error.hpp"
#include "outcome_error_info.hpp"
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

djinni::expected<std::string, outcome_error> TestOutcome::getEnumErrorOutcome() {
    return djinni::make_unexpected(outcome_error::DENIED);
}

djinni::expected<std::string, OutcomeErrorInfo> TestOutcome::getRecordErrorOutcome() {
    return djinni::make_unexpected(OutcomeErrorInfo(404, "not found"));
}

} // namespace testsuite

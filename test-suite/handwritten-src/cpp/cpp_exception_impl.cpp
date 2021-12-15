#include "cpp_exception_impl.hpp"
#include <exception>

namespace testsuite {

int32_t CppExceptionImpl::throw_an_exception() {
    throw ExampleException();
    return 0;
}

int32_t CppExceptionImpl::throw_an_exception_from_js(const std::shared_ptr<JsExceptionThrower>& thrower) {
    thrower->do_throw();
    return 0;
}

std::shared_ptr<CppException> CppException::get() {
    return std::make_shared<CppExceptionImpl>();
}

} // namespace testsuite

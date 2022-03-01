#include "cpp_exception_impl.hpp"
#include "js_exception.hpp"
#include <exception>

namespace testsuite {

int32_t CppExceptionImpl::throw_an_exception() {
    throw ExampleException();
}

int32_t CppExceptionImpl::throw_js_exception(const /*not-null*/ std::shared_ptr<JsException>& cb) {
    cb->throw_js_exception();
    return 0;
}

std::shared_ptr<CppException> CppException::get() {
    return std::make_shared<CppExceptionImpl>();
}

} // namespace testsuite

#include "cpp_exception_impl.hpp"
#include "throwing_interface.hpp"
#include <exception>

namespace testsuite {

int32_t CppExceptionImpl::throw_an_exception() {
    throw ExampleException();
}

int32_t CppExceptionImpl::call_throwing_interface(const /*not-null*/ std::shared_ptr<ThrowingInterface>& cb) {
    cb->throw_exception();
    return 0;
}

std::string CppExceptionImpl::call_throwing_and_catch(const /*not-null*/ std::shared_ptr<ThrowingInterface>& cb) {
    try {
        cb->throw_exception();
    } catch (std::exception& e) {
        return e.what();
    }
    return {};
}

std::shared_ptr<CppException> CppException::get() {
    return std::make_shared<CppExceptionImpl>();
}

} // namespace testsuite

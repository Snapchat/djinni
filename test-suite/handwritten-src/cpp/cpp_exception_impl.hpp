#include "cpp_exception.hpp"
#include "js_exception_thrower.hpp"
#include <exception>

namespace testsuite {

class ExampleException: public std::exception {
    virtual const char* what() const throw() {
        return "Exception Thrown";
    }
};

extern ExampleException EXAMPLE_EXCEPTION;

class CppExceptionImpl : public CppException {
    public:
    CppExceptionImpl() = default;
    ~CppExceptionImpl() = default;

    int32_t throw_an_exception() override;
    int32_t throw_an_exception_from_js(const std::shared_ptr<JsExceptionThrower>& thrower) override;
};

} // namespace testsuite


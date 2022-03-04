#include "cpp_exception.hpp"
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
    CppExceptionImpl() {}
    virtual ~CppExceptionImpl() {}

    virtual int32_t throw_an_exception () override;
    virtual int32_t call_throwing_interface(const /*not-null*/ std::shared_ptr<ThrowingInterface>& cb) override;
    virtual std::string call_throwing_and_catch(const /*not-null*/ std::shared_ptr<ThrowingInterface>& cb) override;
};

} // namespace testsuite


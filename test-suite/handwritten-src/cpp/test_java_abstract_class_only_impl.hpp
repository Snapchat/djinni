#include "test_java_abstract_class_only.hpp"

namespace testsuite {

class TestJavaAbstractClassOnlyImpl: public TestJavaAbstractClassOnly {
public:
public:
    TestJavaAbstractClassOnlyImpl() {}
    virtual ~TestJavaAbstractClassOnlyImpl() {}

    static bool test_method();
};

}  // namespace testsuite

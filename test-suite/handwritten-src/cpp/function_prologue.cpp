#include "FunctionPrologueHelper.hpp"

namespace testsuite {

std::string methodName;

std::string FunctionPrologueHelper::foo() {
    return methodName;
}

}

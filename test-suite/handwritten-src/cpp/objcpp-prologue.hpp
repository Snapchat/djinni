#pragma once

namespace testsuite {

extern std::string methodName;

struct TestPrologue {
    explicit TestPrologue(const char* method) {
        methodName = method;
    }
};

}

#define DJINNI_FUNCTION_PROLOGUE(method) testsuite::TestPrologue testPrologue(method);

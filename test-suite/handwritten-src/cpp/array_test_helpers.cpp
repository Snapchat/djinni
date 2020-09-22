#include "test_array.hpp"
#include "vec2.hpp"

namespace testsuite {

std::vector<std::string> TestArray::testStringArray(const std::vector<std::string>& a) {
    return {a.rbegin(), a.rend()};
}

std::vector<int32_t> TestArray::testIntArray(const std::vector<int32_t>& a) {
    return {a.rbegin(), a.rend()};
}

std::vector<Vec2> TestArray::testRecordArray(const std::vector<Vec2>& a) {
    return {a.rbegin(), a.rend()};
}

std::vector<std::vector<int32_t>> TestArray::testArrayOfArray(const std::vector<std::vector<int32_t>>& a) {
    return {a.rbegin(), a.rend()};
}

} // namespace testsuite

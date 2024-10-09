// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from array.djinni

#include "NativeTestArray.hpp"  // my header
#include "NativeVec2.hpp"

namespace djinni_generated {

djinni::swift::AnyValue TestArray_testStringArray(const djinni::swift::ParameterList* params) try {
    auto _a = ::djinni::swift::Array<::djinni::swift::String>::toCpp(params->getValue(0));
    auto ret = ::testsuite::TestArray::testStringArray(std::move(_a));
    return ::djinni::swift::Array<::djinni::swift::String>::fromCpp(ret);
}
catch (const std::exception& e) {
    return {djinni::swift::ErrorValue{ e.what(), std::current_exception()}};
}
djinni::swift::AnyValue TestArray_testIntArray(const djinni::swift::ParameterList* params) try {
    auto _a = ::djinni::swift::Array<::djinni::swift::I32>::toCpp(params->getValue(0));
    auto ret = ::testsuite::TestArray::testIntArray(std::move(_a));
    return ::djinni::swift::Array<::djinni::swift::I32>::fromCpp(ret);
}
catch (const std::exception& e) {
    return {djinni::swift::ErrorValue{ e.what(), std::current_exception()}};
}
djinni::swift::AnyValue TestArray_testRecordArray(const djinni::swift::ParameterList* params) try {
    auto _a = ::djinni::swift::Array<::djinni_generated::NativeVec2>::toCpp(params->getValue(0));
    auto ret = ::testsuite::TestArray::testRecordArray(std::move(_a));
    return ::djinni::swift::Array<::djinni_generated::NativeVec2>::fromCpp(ret);
}
catch (const std::exception& e) {
    return {djinni::swift::ErrorValue{ e.what(), std::current_exception()}};
}
djinni::swift::AnyValue TestArray_testArrayOfArray(const djinni::swift::ParameterList* params) try {
    auto _a = ::djinni::swift::Array<::djinni::swift::Array<::djinni::swift::I32>>::toCpp(params->getValue(0));
    auto ret = ::testsuite::TestArray::testArrayOfArray(std::move(_a));
    return ::djinni::swift::Array<::djinni::swift::Array<::djinni::swift::I32>>::fromCpp(ret);
}
catch (const std::exception& e) {
    return {djinni::swift::ErrorValue{ e.what(), std::current_exception()}};
}

} // namespace djinni_generated

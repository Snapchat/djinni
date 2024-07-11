// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from client_interface.djinni

#include "NativeReverseClientInterface.hpp"  // my header

namespace djinni_generated {

djinni::swift::AnyValue ReverseClientInterface_returnStr(const djinni::swift::ParameterList* params) try {
    auto inst = NativeReverseClientInterface::toCpp(params->getValue(0));
    auto ret = inst->return_str();
    return ::djinni::swift::String::fromCpp(ret);
}
catch (const std::exception& e) {
    return {djinni::swift::ErrorValue{ e.what(), std::current_exception()}};
}
djinni::swift::AnyValue ReverseClientInterface_methTakingInterface(const djinni::swift::ParameterList* params) try {
    auto inst = NativeReverseClientInterface::toCpp(params->getValue(0));
    auto _i = ::djinni_generated::NativeReverseClientInterface::toCpp(params->getValue(1));
    auto ret = inst->meth_taking_interface(std::move(_i));
    return ::djinni::swift::String::fromCpp(ret);
}
catch (const std::exception& e) {
    return {djinni::swift::ErrorValue{ e.what(), std::current_exception()}};
}
djinni::swift::AnyValue ReverseClientInterface_methTakingOptionalInterface(const djinni::swift::ParameterList* params) try {
    auto inst = NativeReverseClientInterface::toCpp(params->getValue(0));
    auto _i = ::djinni::swift::Optional<std::experimental::optional, ::djinni_generated::NativeReverseClientInterface>::toCpp(params->getValue(1));
    auto ret = inst->meth_taking_optional_interface(std::move(_i));
    return ::djinni::swift::String::fromCpp(ret);
}
catch (const std::exception& e) {
    return {djinni::swift::ErrorValue{ e.what(), std::current_exception()}};
}
djinni::swift::AnyValue ReverseClientInterface_create(const djinni::swift::ParameterList* params) try {
    auto ret = ::testsuite::ReverseClientInterface::create();
    return ::djinni_generated::NativeReverseClientInterface::fromCpp(ret);
}
catch (const std::exception& e) {
    return {djinni::swift::ErrorValue{ e.what(), std::current_exception()}};
}

} // namespace djinni_generated
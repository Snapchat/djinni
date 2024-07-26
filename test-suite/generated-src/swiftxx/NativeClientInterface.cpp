// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from client_interface.djinni

#include "NativeClientInterface.hpp"  // my header
#include "NativeClientReturnedRecord.hpp"

namespace djinni_generated {

djinni::swift::AnyValue ClientInterfaceSwiftProxy::make(void* instance, djinni::swift::DispatchFunc dispatcher) {
    return {std::make_shared<ClientInterfaceSwiftProxy>(instance, dispatcher)};
}
::testsuite::ClientReturnedRecord ClientInterfaceSwiftProxy::get_record(int64_t record_id, const std::string & utf8string, const std::experimental::optional<std::string> & misc) {
    djinni::swift::ParameterList params;
    params.addValue(::djinni::swift::I64::fromCpp(record_id));
    params.addValue(::djinni::swift::String::fromCpp(utf8string));
    params.addValue(::djinni::swift::Optional<std::experimental::optional, ::djinni::swift::String>::fromCpp(misc));
    return ::djinni_generated::NativeClientReturnedRecord::toCpp(callProtocol(0, &params));
}
double ClientInterfaceSwiftProxy::identifier_check(const std::vector<uint8_t> & data, int32_t r, int64_t jret) {
    djinni::swift::ParameterList params;
    params.addValue(::djinni::swift::Binary::fromCpp(data));
    params.addValue(::djinni::swift::I32::fromCpp(r));
    params.addValue(::djinni::swift::I64::fromCpp(jret));
    return ::djinni::swift::F64::toCpp(callProtocol(1, &params));
}
std::string ClientInterfaceSwiftProxy::return_str() {
    djinni::swift::ParameterList params;
    return ::djinni::swift::String::toCpp(callProtocol(2, &params));
}
std::string ClientInterfaceSwiftProxy::meth_taking_interface(const /*not-null*/ std::shared_ptr<::testsuite::ClientInterface> & i) {
    djinni::swift::ParameterList params;
    params.addValue(::djinni_generated::NativeClientInterface::fromCpp(i));
    return ::djinni::swift::String::toCpp(callProtocol(3, &params));
}
std::string ClientInterfaceSwiftProxy::meth_taking_optional_interface(const /*nullable*/ std::shared_ptr<::testsuite::ClientInterface> & i) {
    djinni::swift::ParameterList params;
    params.addValue(::djinni::swift::Optional<std::experimental::optional, ::djinni_generated::NativeClientInterface>::fromCpp(i));
    return ::djinni::swift::String::toCpp(callProtocol(4, &params));
}

} // namespace djinni_generated
// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from test.djinni

#include "NativeEmptyRecord.hpp"  // my header

namespace djinni_generated {

djinni::swift::AnyValue NativeEmptyRecord::fromCpp(const ::testsuite::EmptyRecord& c) {
    auto ret = std::make_shared<djinni::swift::CompositeValue>();
    return {ret};
}
::testsuite::EmptyRecord NativeEmptyRecord::toCpp(const djinni::swift::AnyValue& s) {
    auto p = std::get<djinni::swift::CompositeValuePtr>(s);
    return ::testsuite::EmptyRecord();
}

} // namespace djinni_generated

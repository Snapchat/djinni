// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from array.djinni

#include "NativeVec2.hpp"  // my header

namespace djinni_generated {

djinni::swift::AnyValue NativeVec2::fromCpp(const ::testsuite::Vec2& c) {
    auto ret = std::make_shared<djinni::swift::CompositeValue>();
    ret->addValue(::djinni::swift::I32::fromCpp(c.x));
    ret->addValue(::djinni::swift::I32::fromCpp(c.y));
    return {ret};
}
::testsuite::Vec2 NativeVec2::toCpp(const djinni::swift::AnyValue& s) {
    auto p = std::get<djinni::swift::CompositeValuePtr>(s);
    return ::testsuite::Vec2(::djinni::swift::I32::toCpp(p->getValue(0)), ::djinni::swift::I32::toCpp(p->getValue(1)));
}

} // namespace djinni_generated
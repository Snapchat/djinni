// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from enum.djinni

#import "DBEnumUsageRecord+Private.h"
#import "DBColor+Private.h"
#import "DJIMarshal+Private.h"
#include <cassert>

namespace djinni_generated {

auto EnumUsageRecord::toCpp(ObjcType obj) -> CppType
{
    assert(obj);
    return {::djinni::Enum<::testsuite::color, DBColor>::toCpp(obj.e),
            ::djinni::Optional<std::experimental::optional, ::djinni::Enum<::testsuite::color, DBColor>>::toCpp(obj.o),
            ::djinni::List<::djinni::Enum<::testsuite::color, DBColor>>::toCpp(obj.l),
            ::djinni::Set<::djinni::Enum<::testsuite::color, DBColor>>::toCpp(obj.s),
            ::djinni::Map<::djinni::Enum<::testsuite::color, DBColor>, ::djinni::Enum<::testsuite::color, DBColor>>::toCpp(obj.m)};
}

auto EnumUsageRecord::fromCpp(const CppType& cpp) -> ObjcType
{
    return [[DBEnumUsageRecord alloc] initWithE:(::djinni::Enum<::testsuite::color, DBColor>::fromCpp(cpp.e))
                                              o:(::djinni::Optional<std::experimental::optional, ::djinni::Enum<::testsuite::color, DBColor>>::fromCpp(cpp.o))
                                              l:(::djinni::List<::djinni::Enum<::testsuite::color, DBColor>>::fromCpp(cpp.l))
                                              s:(::djinni::Set<::djinni::Enum<::testsuite::color, DBColor>>::fromCpp(cpp.s))
                                              m:(::djinni::Map<::djinni::Enum<::testsuite::color, DBColor>, ::djinni::Enum<::testsuite::color, DBColor>>::fromCpp(cpp.m))];
}

} // namespace djinni_generated

// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from client_interface.djinni

#import "DBClientReturnedRecord+Private.h"
#import "DJIMarshal+Private.h"
#include <cassert>

namespace djinni_generated {

auto ClientReturnedRecord::toCpp(ObjcType obj) -> CppType
{
    assert(obj);
    return {::djinni::I64::toCpp(obj.recordId),
            ::djinni::String::toCpp(obj.content),
            ::djinni::Optional<std::experimental::optional, ::djinni::String>::toCpp(obj.misc)};
}

auto ClientReturnedRecord::fromCpp(const CppType& cpp) -> ObjcType
{
    return [[DBClientReturnedRecord alloc] initWithRecordId:(::djinni::I64::fromCpp(cpp.record_id))
                                                    content:(::djinni::String::fromCpp(cpp.content))
                                                       misc:(::djinni::Optional<std::experimental::optional, ::djinni::String>::fromCpp(cpp.misc))];
}

} // namespace djinni_generated

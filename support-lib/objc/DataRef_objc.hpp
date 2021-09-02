#pragma once

#include "DJIMarshal+Private.h"
#include "DataRef.hpp"

namespace snapchat {
namespace djinni {
struct NativeDataRef {
    using CppType = DataRef;
    using ObjcType = NSData*;

    static CppType toCpp(ObjcType data) {
        if ([data isKindOfClass:[NSMutableData class]]) {
            return DataRef((__bridge CFMutableDataRef)data);
        } else {
            return DataRef((__bridge CFDataRef)data);
        }
    }

    static ObjcType fromCpp(const CppType& c) {
        return (__bridge NSData*)c.platformObj();
    }

    using Boxed = NativeDataRef;
};
}} // namespace snapchat::djinni

#pragma once

#include "DJIMarshal+Private.h"
#include "DataView.hpp"

namespace snapchat {
namespace djinni {

struct NativeDataView {
    using CppType = DataView;
    using ObjcType = NSData*;

    static CppType toCpp(ObjcType data) {
        // reinterpret_cast changed because this is no longer const...
        return DataView(reinterpret_cast<uint8_t*>(const_cast<void*>(data.bytes)), data.length);
    }

    static ObjcType fromCpp(const CppType& c) {
        return [NSData dataWithBytesNoCopy:c.buf() length:c.len() freeWhenDone:NO];
    }

    using Boxed = NativeDataView;
};

}} // namespace snapchat::djinni

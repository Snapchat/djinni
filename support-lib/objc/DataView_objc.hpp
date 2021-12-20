/**
  * Copyright 2021 Snap, Inc.
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *    http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

#pragma once

#include "DJIMarshal+Private.h"
#include "../cpp/DataView.hpp"

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

} // namespace djinni

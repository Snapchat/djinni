/**
  * Copyright 2024 Snap, Inc.
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

#include "djinni_valdi.hpp"
#include "../cpp/DataRef.hpp"

namespace djinni::valdi {

struct NativeDataRef {
    using CppType = DataRef;
    using ValdiType = Valdi::Value;
    using Boxed = NativeDataRef;

    static CppType toCpp(const ValdiType& v);
    static ValdiType fromCpp(const CppType& c);
    
    static const Valdi::ValueSchema& schema();
};

} // namespace djinni

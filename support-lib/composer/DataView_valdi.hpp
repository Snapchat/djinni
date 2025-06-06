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
#include "../cpp/DataView.hpp"

namespace djinni::valdi {

struct NativeDataView {
    using CppType = DataView;
    using ValdiType = Valdi::Value;
    using Boxed = NativeDataView;

    static CppType toCpp(const ValdiType& v) {
        auto arr = v.getTypedArrayRef();
        const auto& buf = arr->getBuffer();
        return DataView(buf.data(), buf.size());
    }

    static ValdiType fromCpp(const CppType& c) {
        Valdi::BytesView buf(nullptr, c.buf(), c.len());
        auto arr = Valdi::makeShared<Valdi::ValueTypedArray>(Valdi::kDefaultTypedArrayType, buf);
        return Valdi::Value(arr);
    }
    
    static const Valdi::ValueSchema& schema() {
        static auto schema = Valdi::ValueSchema::valueTypedArray();
        return schema;
    }
};

} // namespace djinni

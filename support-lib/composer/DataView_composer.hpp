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

#include "djinni_composer.hpp"
#include "DataView.hpp"

namespace djinni::composer {

struct NativeDataView {
    using CppType = DataView;
    using ComposerType = Composer::Value;
    using Boxed = NativeDataView;

    static CppType toCpp(const ComposerType& v) {
        auto arr = v.getTypedArrayRef();
        const auto& buf = arr->getBuffer();
        return DataView(buf.data(), buf.size());
    }

    static ComposerType fromCpp(const CppType& c) {
        Composer::BytesView buf(nullptr, c.buf(), c.len());
        auto arr = Composer::makeShared<Composer::ValueTypedArray>(Composer::kDefaultTypedArrayType, buf);
        return Composer::Value(arr);
    }
    
    static const Composer::ValueSchema& schema() {
        static auto schema = Composer::ValueSchema::valueTypedArray();
        return schema;
    }
};

} // namespace djinni

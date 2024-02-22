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
#include "expected.hpp"
#import "DJOutcome.h"

namespace djinni {

template<typename RESULT, typename ERROR>
class Outcome {
    using ResultCppType = typename RESULT::CppType;
    using ErrorCppType = typename ERROR::CppType;
    using ResultObjcType = typename RESULT::Boxed::ObjcType;
    using ErrorObjcType = typename ERROR::Boxed::ObjcType;
public:
    using CppType = expected<ResultCppType, ErrorCppType>;
    using ObjcType = DJOutcome*;

    using Boxed = Outcome;

    static CppType toCpp(ObjcType o) {
        assert(o);
        ResultObjcType r = [o result];
        if (r) {
            return RESULT::Boxed::toCpp(r);
        } else {
            ErrorObjcType e = [o error];
            return make_unexpected(ERROR::Boxed::toCpp(e));
        }
    }

    static ObjcType fromCpp(const CppType& c) {
        if (c.has_value()) {
            return [DJOutcome fromResult: RESULT::Boxed::fromCpp(c.value())];
        } else {
            return [DJOutcome fromError: ERROR::Boxed::fromCpp(c.error())];
        }
    }
};

} // namespace djinni

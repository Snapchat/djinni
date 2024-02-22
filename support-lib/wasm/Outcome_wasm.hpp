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

#include "djinni_wasm.hpp"
#include "expected.hpp"

namespace djinni {

// js success type:
// { result: ...}
// js error type:
// { error: ...}
template <class Result, class Error>
struct Outcome
{
    using CppType = expected<typename Result::CppType, typename Error::CppType>;
    using JsType = em::val;
    using Boxed = Outcome;
    
    static CppType toCpp(const JsType& j) {
        em::val res = j["result"];
        if (!res.isUndefined()) {
            return Result::Boxed::toCpp(res);
        } else {
            em::val err = j["error"];
            assert(!err.isUndefined());
            return make_unexpected(Error::Boxed::toCpp(err));
        }
    }
    static JsType fromCpp(const CppType& c) {
        if (c.has_value()) {
            em::val res = em::val::object();
            res.set("result", Result::Boxed::fromCpp(c.value()));
            return res;
        } else {
            em::val err = em::val::object();
            err.set("error", Error::Boxed::fromCpp(c.error()));
            return err;
        }
    }
};

} // namespace djinni

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

#include "Lazy_c.h"
#include <functional>

namespace djinni::c_api {

template <typename Tr>
struct LazyTranslator {
    using CppType = std::function<typename Tr::CppType()>;
    using CType = djinni_lazy_ref;

    static CppType toCpp(djinni_lazy_ref lazy) {
        djinni_ref_retain(lazy);
        return [lazy]() -> typename Tr::CppType {
            auto result_c = djinni_lazy_call(lazy);
            auto result_cpp = Tr::toCpp(result_c);
            djinni_ref_release(result_c);
            djinni_ref_release(lazy);
            return result_cpp;
        };
    }

    static CType fromCpp(const CppType& lazy) {
        auto* func_ptr = new CppType(lazy);
        return djinni_lazy_make(
            [](void* context) -> djinni_ref {
                auto lazy_func = static_cast<CppType*>(context);
                auto result = (*lazy_func)();
                return Tr::fromCpp(std::move(result));
            },
            func_ptr,
            [](void* context) {
                delete static_cast<CppType*>(context);
            }
        );
    }

    static CType fromCpp(CppType&& lazy) {
        auto* func_ptr = new CppType(std::move(lazy));
        return djinni_lazy_make(
            [](void* context) -> djinni_ref {
                auto lazy_func = static_cast<CppType*>(context);
                auto result = (*lazy_func)();
                return Tr::fromCpp(std::move(result));
            },
            func_ptr,
            [](void* context) {
                delete static_cast<CppType*>(context);
            }
        );
    }
};

} // namespace djinni::c_api

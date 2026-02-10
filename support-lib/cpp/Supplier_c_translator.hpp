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

#include "Supplier_c.h"
#include <functional>

namespace djinni::c_api {

template <typename Tr>
struct SupplierTranslator {
    using CppType = std::function<typename Tr::CppType()>;
    using CType = djinni_supplier_ref;

    static CppType toCpp(djinni_supplier_ref supplier) {
        djinni_ref_retain(supplier);
        return [supplier]() -> typename Tr::CppType {
            auto result_c = djinni_supplier_call(supplier);
            auto result_cpp = Tr::toCpp(result_c);
            djinni_ref_release(result_c);
            djinni_ref_release(supplier);
            return result_cpp;
        };
    }

    static CType fromCpp(const CppType& supplier) {
        auto* func_ptr = new CppType(supplier);
        return djinni_supplier_make(
            [](void* context) -> djinni_ref {
                auto supplier_func = static_cast<CppType*>(context);
                auto result = (*supplier_func)();
                return Tr::fromCpp(std::move(result));
            },
            func_ptr,
            [](void* context) {
                delete static_cast<CppType*>(context);
            }
        );
    }

    static CType fromCpp(CppType&& supplier) {
        auto* func_ptr = new CppType(std::move(supplier));
        return djinni_supplier_make(
            [](void* context) -> djinni_ref {
                auto supplier_func = static_cast<CppType*>(context);
                auto result = (*supplier_func)();
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

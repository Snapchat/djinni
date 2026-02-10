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

#include "djinni_support.hpp"
#include <functional>

namespace djinni::swift {

// Callback from Swift to call a C++ supplier function
typedef AnyValue (*SupplierFuncCb)(void* ctx);

// Function to wrap a Swift supplier function for C++ use
AnyValue callSwiftSupplierFunction(const AnyValue& supplierValue);

// Holder for a C++ supplier function
template<typename RESULT>
struct CppSupplierHolder: CallableSupplier {
    using CppResType = typename RESULT::CppType;
    std::function<CppResType()> func;

    explicit CppSupplierHolder(std::function<CppResType()> f) : func(std::move(f)) {}

    AnyValue call() override {
        if constexpr (std::is_void_v<CppResType>) {
            func();
            return makeVoidValue();
        } else {
            return RESULT::fromCpp(func());
        }
    }
};

// Holder for a Swift supplier function
struct SwiftSupplierHolder: CallableSupplier {
    AnyValue supplierValue;

    explicit SwiftSupplierHolder(const AnyValue& v) : supplierValue(v) {}

    AnyValue call() override {
        return callSwiftSupplierFunction(supplierValue);
    }
};

template <class RESULT>
class SupplierAdaptor {
    using CppResType = typename RESULT::CppType;

public:
    using CppType = std::function<CppResType()>;

    static CppType toCpp(const AnyValue& o) {
        // Check if this is already a C++ supplier function
        auto cppHolder = std::dynamic_pointer_cast<CppSupplierHolder<RESULT>>(std::get<OpaqueValuePtr>(o));
        if (cppHolder) {
            return std::move(cppHolder->func);
        }

        // It's a Swift supplier function
        auto swiftHolder = std::make_shared<SwiftSupplierHolder>(o);

        return [swiftHolder]() -> CppResType {
            auto result = swiftHolder->call();
            if constexpr (std::is_void_v<CppResType>) {
                return;
            } else {
                return RESULT::toCpp(result);
            }
        };
    }

    static AnyValue fromCpp(const CppType& c) {
        auto holder = std::make_shared<CppSupplierHolder<RESULT>>(c);
        OpaqueValuePtr ptr = holder;
        return {ptr};
    }

    static AnyValue fromCpp(CppType&& c) {
        auto holder = std::make_shared<CppSupplierHolder<RESULT>>(std::move(c));
        OpaqueValuePtr ptr = holder;
        return {ptr};
    }
};

} // namespace djinni::swift

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
#import "DJSupplier.h"
#include <functional>

namespace djinni {

/**
 * Supplier<T> type adapter for Objective-C.
 *
 * Maps C++ std::function<T()> to Objective-C DJSupplier<T> generic class
 *
 * The ObjcType is 'DJSupplier<T> *' which provides type-safe supplier evaluation.
 * Users call the -get method to get the value: T result = [supplier get];
 *
 * Analogous to Java's Supplier<T> interface.
 *
 * Example:
 *   // From Objective-C to C++
 *   DJSupplier<NSString *> *supplierStr = [DJSupplier supplierWithBlock:^NSString *() {
 *       return @"computed value";
 *   }];
 *
 *   // From C++ to Objective-C
 *   DJSupplier<NSString *> *objcSupplier = SupplierAdaptor::fromCpp(cppSupplier);
 *   NSString *result = [objcSupplier get];
 */
template<typename T>
class SupplierAdaptor {
    using CppType = std::function<typename T::CppType()>;
    using CppResType = typename T::CppType;
    using ObjcResType = typename T::Boxed::ObjcType;

public:
    using ObjcType = id;  // DJSupplier<T> *
    using Boxed = SupplierAdaptor;

    static CppType toCpp(ObjcType o) {
        assert(o);
        // Capture the DJSupplier object
        id supplierCopy = o;  // ARC will handle reference counting

        return [supplierCopy]() -> CppResType {
            // Get the supplier value
            ObjcResType result = [supplierCopy get];
            return T::Boxed::toCpp(result);
        };
    }

    static ObjcType fromCpp(const CppType& c) {
        // Create a DJSupplier that wraps the C++ function
        auto lambda_ptr = std::make_shared<CppType>(c);

        ObjcResType (^block)(void) = ^ObjcResType() {
            CppResType result = (*lambda_ptr)();
            return T::Boxed::fromCpp(result);
        };

        // Create and return a DJSupplier instance
        return [DJSupplier supplierWithBlock:block];
    }
};

} // namespace djinni

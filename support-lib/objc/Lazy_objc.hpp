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
#import "DJLazy.h"
#include <functional>

namespace djinni {

/**
 * Lazy<T> type adapter for Objective-C.
 * 
 * Maps C++ std::function<T()> to Objective-C DJLazy<T> generic class
 * 
 * The ObjcType is 'DJLazy<T> *' which provides type-safe lazy evaluation.
 * Users call the -get method to get the lazy value: T result = [lazy get];
 * 
 * Analogous to Java's Supplier<T> interface.
 * 
 * Example:
 *   // From Objective-C to C++
 *   DJLazy<NSString *> *lazyStr = [DJLazy lazyWithBlock:^NSString *() {
 *       return @"computed value";
 *   }];
 *   
 *   // From C++ to Objective-C  
 *   DJLazy<NSString *> *objcLazy = LazyAdaptor::fromCpp(cppLazy);
 *   NSString *result = [objcLazy get];
 */
template<typename T>
class LazyAdaptor {
    using CppType = std::function<typename T::CppType()>;
    using CppResType = typename T::CppType;
    using ObjcResType = typename T::Boxed::ObjcType;
    
public:
    using ObjcType = id;  // DJLazy<T> *
    using Boxed = LazyAdaptor;

    static CppType toCpp(ObjcType o) {
        assert(o);
        // Capture the DJLazy object
        id lazyCopy = o;  // ARC will handle reference counting
        
        return [lazyCopy]() -> CppResType {
            // Get the lazy value
            ObjcResType result = [lazyCopy get];
            return T::Boxed::toCpp(result);
        };
    }

    static ObjcType fromCpp(const CppType& c) {
        // Create a DJLazy that wraps the C++ function
        auto lambda_ptr = std::make_shared<CppType>(c);
        
        ObjcResType (^block)(void) = ^ObjcResType() {
            CppResType result = (*lambda_ptr)();
            return T::Boxed::fromCpp(result);
        };
        
        // Create and return a DJLazy instance
        return [DJLazy lazyWithBlock:block];
    }
};

} // namespace djinni

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

#include "Lazy_swift.hpp"

namespace djinni::swift {

// This function is called from C++ to invoke a Swift lazy function
// The actual implementation needs to be provided by Swift side
AnyValue callSwiftLazyFunction(const AnyValue& lazyValue) {
    // This would typically call into Swift through a registered callback
    // For now, we'll just return a void value as a placeholder
    // The actual implementation would be in the Swift support code
    return makeVoidValue();
}

} // namespace djinni::swift

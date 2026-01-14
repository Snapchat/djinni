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

import DjinniSupportCxx
import Foundation

// `lazy<T>` is marshalled as a function value that returns T
public enum LazyMarshaller<T: Marshaller>: Marshaller {
    public typealias SwiftType = () -> T.SwiftType
    
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        // The C++ side sends us a function object
        // We need to capture it and create a Swift closure
        let cppFunc = v
        return {
            let result = djinni.swift.callLazyFunction(cppFunc)
            return T.fromCpp(result)
        }
    }
    
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        // Create a C++ function object from the Swift closure
        return djinni.swift.makeLazyFunction { () -> djinni.swift.AnyValue in
            let result = s()
            return T.toCpp(result)
        }
    }
}

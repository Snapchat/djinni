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

/// Supplier of a value: equivalent to `() -> T`. Used by the Djinni `supplier<T>` type
/// so generated code has a concrete nominal type (e.g. `Supplier<Int32>`).
public struct Supplier<T> {
    private let fn: () -> T

    public init(_ fn: @escaping () -> T) {
        self.fn = fn
    }

    public func callAsFunction() -> T {
        fn()
    }
}

// Box to hold a Swift closure so we can pass it to C++ as (callback, context).
private final class SupplierClosureBox {
    let fn: () -> djinni.swift.AnyValue
    init(_ fn: @escaping () -> djinni.swift.AnyValue) { self.fn = fn }
}

private func supplierClosureCall(ctx: UnsafeMutableRawPointer?) -> djinni.swift.AnyValue {
    let box = Unmanaged<SupplierClosureBox>.fromOpaque(ctx!).takeUnretainedValue()
    return box.fn()
}

private func supplierClosureRelease(ctx: UnsafeMutableRawPointer?) {
    Unmanaged<SupplierClosureBox>.fromOpaque(ctx!).release()
}

// `supplier<T>` is marshalled as Supplier<T> (type-alias style, like TypeScript's Supplier<T> = () => T)
public enum SupplierMarshaller<T: Marshaller>: Marshaller {
    public typealias SwiftType = Supplier<T.SwiftType>

    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        let cppFunc = v
        return Supplier {
            let result = djinni.swift.callSupplierFunction(cppFunc)
            return T.fromCpp(result)
        }
    }

    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        let box = SupplierClosureBox {
            let result = s.callAsFunction()
            return T.toCpp(result)
        }
        let boxPtr = Unmanaged.passRetained(box).toOpaque()
        return djinni.swift.makeSupplierFunction(supplierClosureCall, boxPtr, supplierClosureRelease)
    }
}

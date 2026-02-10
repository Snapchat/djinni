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

import Foundation

/// Type alias for a supplier of a value: equivalent to `() -> T` (like TypeScript's `Supplier<T> = () => T`).
/// Used by the Djinni `supplier<T>` type so generated code has a concrete nominal type (e.g. `Supplier<Int32>`).
public struct Supplier<T> {
    private let fn: () -> T

    public init(_ fn: @escaping () -> T) {
        self.fn = fn
    }

    public func callAsFunction() -> T {
        fn()
    }
}

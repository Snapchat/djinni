/**
 * Copyright 2025 Snap, Inc.
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

#import <Foundation/Foundation.h>

/**
 * DJLazy represents a deferred computation - a value that will be computed when needed.
 * 
 * This is used for lazy evaluation, allowing expensive computations or object
 * creations to be deferred until actually needed.
 *
 * Analogous to Java's Supplier<T> interface.
 *
 * Example:
 *   DJLazy<NSString *> *lazyValue = [DJLazy lazyWithBlock:^NSString *() {
 *       return @"computed value";
 *   }];
 *   NSString *result = [lazyValue get];  // Compute and get the value
 */
@interface DJLazy<__covariant Value> : NSObject

- (nonnull instancetype)init NS_UNAVAILABLE;

/**
 * Creates a lazy value from a block.
 * The block will be called when `get` is invoked.
 */
+ (nonnull instancetype)lazyWithBlock:(Value _Nonnull (^_Nonnull)(void))block;

/**
 * Initializes a lazy value with a block.
 * The block will be called when `get` is invoked.
 */
- (nonnull instancetype)initWithBlock:(Value _Nonnull (^_Nonnull)(void))block NS_DESIGNATED_INITIALIZER;

/**
 * Gets the lazy value by executing the computation.
 * The block is executed each time this is called (no memoization by default).
 * Analogous to Java's Supplier.get() method.
 */
- (Value _Nonnull)get;

@end

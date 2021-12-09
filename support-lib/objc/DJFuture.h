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

#import <Foundation/Foundation.h>

@class DJFuture;

@interface DJFuture<__covariant Value> : NSObject
-(nonnull instancetype) init NS_UNAVAILABLE;
// If the future is ready, then calling its `get` method will not block.
-(BOOL) isReady;
// Wait until future becomes ready
-(void) wait;
// Block and wait for the result (or exception). This can only be called once.
-(nullable Value) get;
// Tell the future to Call the specified handler routine when it becomes
// ready. Returns a new future that wraps the return value of the handler
// routine. The current future becomes invalid after this call.
-(nonnull DJFuture<id>*)then:(_Nullable id(^_Nonnull)(DJFuture<Value>* _Nonnull))handler;
@end

// ------------------------------------------

@interface DJPromise<Value> : NSObject
-(nonnull instancetype) init;
// Get a future object associated with this promise
-(nonnull DJFuture<Value>*) getFuture;
// `setValue` or `setException` can only be called once on a promise. After
// which the internal state becomes invalid.
-(void) setValue:(nullable Value) val;
-(void) setException:(nonnull NSException*) exception;
@end

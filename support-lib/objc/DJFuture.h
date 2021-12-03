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
-(BOOL) isReady;
-(nullable Value) get;
-(nonnull DJFuture<id>*)then:(_Nullable id(^_Nonnull)(DJFuture<Value>* _Nonnull))handler;
@end

// ------------------------------------------

@interface DJPromise<Value> : NSObject
-(nonnull instancetype) init;
-(nonnull DJFuture<Value>*) getFuture;
-(void) setValue:(nullable Value) val;
-(void) setException:(nonnull NSException*) exception;
@end
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

#import "DJFuture.h"

@class DJSharedSate<Value>;

typedef _Nullable id (^Continuation)(DJSharedSate* _Nonnull);

@interface DJSharedSate<Value> : NSObject
    @property (nonatomic, strong) Value value;
    @property (nonatomic, strong) NSException* exception;
    @property (nonatomic, strong) NSCondition* cond;
    @property (nonatomic, strong) Continuation handler;

    @property (readonly) BOOL isReady;
@end

@implementation DJSharedSate
-(instancetype)init {
    if (self = [super init]) {
        self->_value = nil;
        self->_exception = nil;
        self->_cond = [[NSCondition alloc] init];
        self->_handler = nil;
    }
    return self;
}
-(BOOL) isReady {
    return _value != nil || _exception != nil;
}
@end

// ------------------------------------------

@implementation DJFuture {
    DJSharedSate* _sharedState;
}

-(instancetype)initWithSharedState:(DJSharedSate*) sharedState {
    if (self = [super init]) {
        self->_sharedState = sharedState;
    }
    return self;
}

-(BOOL) isReady {
    [_sharedState.cond lock];
    BOOL ready = (_sharedState.isReady);
    [_sharedState.cond unlock];
    return ready;
}

-(id) get {
    id ret = nil;
    id ex = nil;
    [_sharedState.cond lock];
    while (!_sharedState.isReady)
        [_sharedState.cond wait];
    ret = _sharedState.value;
    ex = _sharedState.exception;
    [_sharedState.cond unlock];
    if (ex != nil) {
        @throw ex;
    }
    return ret;
}

-(DJFuture<id>*)then:(_Nullable id(^_Nonnull)(DJFuture<id>* _Nonnull))handler {
    DJPromise<id>* nextPromise = [[DJPromise alloc] init];
    DJFuture<id>* nextFuture = [nextPromise getFuture];
    Continuation continuation;
    continuation = ^id (DJSharedSate* _Nonnull st) {
        @try {
            [nextPromise setValue:handler([[DJFuture alloc] initWithSharedState:st])];
        } @catch ( NSException* e ) {
            [nextPromise setException:e];
        }
        return nil;
    };
    [_sharedState.cond lock];
    DJSharedSate<id>* sharedState = _sharedState;
    _sharedState = nil;
    if (sharedState.isReady) {
        continuation(sharedState.value);
    } else {
        sharedState.handler = continuation;
    }
    [sharedState.cond unlock];
    return nextFuture;
}

@end

// ------------------------------------------

@implementation DJPromise {
    DJSharedSate* _sharedState;
}
-(instancetype) init {
    if (self = [super init]) {
        _sharedState =[[DJSharedSate alloc] init];
    }
    return self;
}

-(DJFuture<id>*) getFuture {
    return [[DJFuture alloc] initWithSharedState: _sharedState];
}

-(void) setValue:(id) val {
    DJSharedSate* sharedState = nil;
    [_sharedState.cond lock];
    sharedState = _sharedState;
    sharedState.value = val;
    [sharedState.cond broadcast];
    [sharedState.cond unlock];
    if (sharedState.handler) {
        sharedState.handler(sharedState);
    }
}

-(void) setException:(NSException*) exception {
    DJSharedSate* sharedState = nil;
    [_sharedState.cond lock];
    sharedState = _sharedState;
    sharedState.exception = exception;
    [sharedState.cond broadcast];
    [sharedState.cond unlock];
    if (sharedState.handler) {
        sharedState.handler(sharedState);
    }
}

@end

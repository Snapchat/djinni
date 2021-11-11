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

@interface DJSharedSate<Value> : NSObject
    @property (nonatomic, strong) Value value;
    @property (nonatomic, strong) NSCondition* cond;
    @property (nonatomic, strong) FutureHandler handler;
@end

@implementation DJSharedSate
-(instancetype)init {
    if (self = [super init]) {
        self->_value = nil;
        self->_cond = [[NSCondition alloc] init];
        self->_handler = nil;
    }
    return self;
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
    BOOL ready = (_sharedState.value != nil);
    [_sharedState.cond unlock];
    return ready;
}

-(id) get {
    id ret = nil;
    [_sharedState.cond lock];
    while (_sharedState.value == nil)
        [_sharedState.cond wait];
    ret = _sharedState.value;
    [_sharedState.cond unlock];
    return ret;
}

-(DJFuture<id>*)then:(FutureHandler)handler {
    DJPromise<id>* nextPromise = [[DJPromise alloc] init];
    DJFuture<id>* nextFuture = [nextPromise getFuture];
    FutureHandler continuation;
    continuation = ^id _Nonnull(id _Nonnull value) {
        [nextPromise setValue:handler(value)];
        return {};
    };
    
    [_sharedState.cond lock];
    if (_sharedState.value != nil) {
        continuation(_sharedState.value);
    } else {
        _sharedState.handler = continuation;
    }
    [_sharedState.cond unlock];
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

-(DJFuture*) getFuture {
    return [[DJFuture alloc] initWithSharedState: _sharedState];
}

-(void) setValue:(id) val {
    [_sharedState.cond lock];
    _sharedState.value = val;
    if (_sharedState.handler) {
        // TODO call out of lock
        _sharedState.handler(val);
    }
    [_sharedState.cond broadcast];
    [_sharedState.cond unlock];
}

@end

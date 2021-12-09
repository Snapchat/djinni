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

template <typename T>
T withLockHeld(id<NSLocking> lock, T(^block)()) {
    [lock lock];
    @try {
        return block();
    } @finally {
        [lock unlock];
    }
}

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
    DJSharedSate* sharedState = nil;
    @synchronized(self) {sharedState = self->_sharedState;}
    return withLockHeld(sharedState.cond, ^{return sharedState.isReady;});
}

-(void) wait {
    DJSharedSate* sharedState = nil;
    @synchronized(self) {sharedState = self->_sharedState;}
    return withLockHeld(sharedState.cond, ^{while (!sharedState.isReady) [sharedState.cond wait];});
}

-(id) get {
    DJSharedSate* sharedState = nil;
    @synchronized(self) {
        sharedState = self->_sharedState;
        self->_sharedState = nil;
    }
    return withLockHeld(sharedState.cond, ^{
            while (!sharedState.isReady) [sharedState.cond wait];
            if (sharedState.exception != nil) {
                @throw sharedState.exception;
            }
            return sharedState.value;
        });
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
    DJSharedSate* sharedState = nil;
    @synchronized(self) {
        sharedState = self->_sharedState;
        self->_sharedState = nil;
    }

    __block DJSharedSate* sharedStateForReadyFuture = nil;
    withLockHeld(sharedState.cond, ^{
            if (sharedState.isReady) {
                sharedStateForReadyFuture = sharedState;
            } else {
                sharedState.handler = continuation;
            }
        });
    if (sharedStateForReadyFuture) {
        continuation(sharedStateForReadyFuture);
    }
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
    @synchronized (self) {
        return [[DJFuture alloc] initWithSharedState: _sharedState];
    }
}

-(void) updateAndCallResultHandler:(void(^)(DJSharedSate*))block {
    DJSharedSate* sharedState = nil;
    @synchronized (self) {
        sharedState = self->_sharedState;
        self->_sharedState = nil;
    }
    __block Continuation handler = nil;    
    withLockHeld(sharedState.cond, ^{
            block(sharedState);
            handler = sharedState.handler;
            if (!handler) {
                [sharedState.cond broadcast];
            }
        });
    if (handler) {
        sharedState.handler(sharedState);
    }
}

-(void) setValue:(id) val {
    [self updateAndCallResultHandler: ^(DJSharedSate* sharedState){sharedState.value = val;}];
}

-(void) setException:(NSException*) exception {
    [self updateAndCallResultHandler: ^(DJSharedSate* sharedState){sharedState.exception = exception;}];
}

@end

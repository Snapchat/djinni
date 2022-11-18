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

@class DJSharedSate<DJValue>;

typedef _Nullable id (^Continuation)(DJSharedSate* _Nonnull);

@interface DJSharedSate<DJValue> : NSObject
@property (nonatomic) DJValue value;
@property (nonatomic) NSException *exception;
@property (nonatomic) NSCondition *cond;
@property (nonatomic) Continuation handler;
@property (nonatomic) BOOL ready;
@property (readonly) BOOL isReady;
@end

@implementation DJSharedSate
- (instancetype)init {
    if (self = [super init]) {
        _cond = [[NSCondition alloc] init];
        _ready = NO;
    }
    return self;
}
- (BOOL)isReady {
    return _ready || _exception != nil;
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
    DJSharedSate *_sharedState;
}

- (instancetype)initWithSharedState:(DJSharedSate *)sharedState {
    if (self = [super init]) {
        self->_sharedState = sharedState;
    }
    return self;
}

- (BOOL)isReady {
    DJSharedSate *sharedState = nil;
    @synchronized(self) {sharedState = self->_sharedState;}
    return withLockHeld(sharedState.cond, ^{return sharedState.isReady;});
}

- (id)get {
    DJSharedSate *sharedState = nil;
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

- (DJFuture<id>*)then:(_Nullable id(^_Nonnull)(DJFuture<id> * _Nonnull))handler {
    DJPromise<id> *nextPromise = [[DJPromise alloc] init];
    DJFuture<id> *nextFuture = [nextPromise getFuture];
    Continuation continuation;
    continuation = ^id (DJSharedSate * _Nonnull st) {
        @try {
            [nextPromise setValue:handler([[DJFuture alloc] initWithSharedState:st])];
        } @catch (NSException *e) {
            [nextPromise setException:e];
        }
        return nil;
    };
    DJSharedSate *sharedState = nil;
    @synchronized(self) {
        sharedState = self->_sharedState;
        self->_sharedState = nil;
    }

    __block DJSharedSate *sharedStateForReadyFuture = nil;
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
    DJSharedSate *_sharedState;
    DJSharedSate *_sharedStateReadonly;
}
- (instancetype)init {
    if (self = [super init]) {
        _sharedState = [[DJSharedSate alloc] init];
        _sharedStateReadonly = _sharedState;
    }
    return self;
}

- (DJFuture<id> *)getFuture {
    @synchronized (self) {
        return [[DJFuture alloc] initWithSharedState: _sharedStateReadonly];
    }
}

- (void)updateAndCallResultHandler:(void(^)(DJSharedSate *))block {
    DJSharedSate *sharedState = nil;
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

- (void)setValue:(id)val {
    [self updateAndCallResultHandler: ^(DJSharedSate *sharedState){
            sharedState.value = val;
            sharedState.ready = YES;
        }];
}

- (void)setValue {
    [self setValue:[NSNull null]];
}

- (void)setException:(NSException *)exception {
    [self updateAndCallResultHandler: ^(DJSharedSate *sharedState){sharedState.exception = exception;}];
}

@end

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

#import "DJOutcome.h"

typedef id (^HANDLER)(id _Nonnull);

@implementation DJOutcome {
    id (^_accessor) (HANDLER _Nonnull, HANDLER _Nonnull);
}
-(instancetype)initWithResult:(id)result {
    if (self = [super init]) {
        self->_accessor = ^(HANDLER resultHandler, HANDLER errorHandler) {return resultHandler(result);};}
    return self;
}
-(instancetype)initWithError:(id)error {
    if (self = [super init]) {
        self->_accessor = ^(HANDLER resultHandler, HANDLER errorHandler) {return errorHandler(error);};}
    return self;
}
+(DJOutcome<id, id>*)fromResult:(id)result {
    return [[DJOutcome alloc] initWithResult:result];
}
+(DJOutcome<id, id>*)fromError:(id)error {
    return [[DJOutcome alloc] initWithError:error];
}
-(id)matchResult:(HANDLER) resultHandler Error:(HANDLER) errorHandler {
    return _accessor(resultHandler, errorHandler);
}
-(id)result {
    return _accessor(^(id result){ return result; }, 
                     ^(id error) { return (id)nil; });
}
-(id)resultOr:(id)defaultResult {
    return _accessor(^(id result){ return result; },
                     ^(id error) { return defaultResult; });
}
-(id)error {
    return _accessor(^(id result){ return (id)nil; },
                     ^(id error) { return error; });
}
- (NSString *)description {
    return _accessor(^(id result){ return [NSString stringWithFormat:@"Outcome:result[%@]", result]; },
                     ^(id error) { return [NSString stringWithFormat:@"Outcome:error[%@]", error]; });
}
-(BOOL)isEqualToOutcome:(DJOutcome*)other {
    NSNumber* ret = _accessor(^(id _Nonnull result){ return [NSNumber numberWithBool:[result isEqual:[other result]]]; },
                              ^(id _Nonnull error) { return [NSNumber numberWithBool:[error isEqual:[other error]]]; });
    return [ret boolValue];
}
-(BOOL)isEqual:(id)other {
    if (other == self)
        return YES;
    if (!other || ![other isKindOfClass:[self class]])
        return NO;
    return [self isEqualToOutcome:other];
}
// stole from boost::hash_combine (https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine)
static inline void hash_combine(NSUInteger& seed, NSUInteger h)
{
    seed ^= h + 0x9e3779b9 + (seed<<6) + (seed>>2);
}
- (NSUInteger)hash {
    __block NSUInteger seed = [[self class] hash];
    return [_accessor(
        ^(id result) {
            hash_combine(seed, 1);
            hash_combine(seed, [result hash]);
            return @(seed);
        },
        ^(id error) {
            hash_combine(seed, 0);
            hash_combine(seed, [error hash]);
            return @(seed);
        }) unsignedIntegerValue];
}
-(id)copyWithZone:(NSZone *)zone
{
    return _accessor(^(id result){ return [DJOutcome fromResult:result]; }, 
                     ^(id error) { return [DJOutcome fromError:error]; });
}
@end

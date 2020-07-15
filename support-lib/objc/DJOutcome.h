#pragma once

#import <Foundation/Foundation.h>

@interface DJOutcome<Result, Error> : NSObject
-(instancetype _Nonnull)init NS_UNAVAILABLE;
-(id _Nullable)matchResult:(id _Nullable (^ _Nonnull)(Result _Nonnull)) resultHandler
                     Error:(id _Nullable (^ _Nonnull)(Error _Nonnull)) errorHandler;
-(Result _Nullable)result;
-(Result _Nonnull)resultOr:(Result _Nullable)defaultResult;
-(Error _Nullable)error;
-(BOOL)isEqualToOutcome:(DJOutcome* _Nullable)other;
+(DJOutcome<Result,Error>* _Nonnull)fromResult:(Result _Nonnull)result;
+(DJOutcome<Result,Error>* _Nonnull)fromError:(Error _Nonnull)error;
@end

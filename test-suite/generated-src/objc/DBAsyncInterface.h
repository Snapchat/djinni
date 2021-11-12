#import <Foundation/Foundation.h>
#import "DJFuture.h"
@protocol DBAsyncInterface;

@protocol DBAsyncInterface <NSObject>

- (nonnull DJFuture<NSNumber*>*) getAsyncResult;

@end

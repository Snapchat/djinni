// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from proto.djinni

#import "proto/objc/test.pbobjc.h"
#import <Foundation/Foundation.h>

@interface DBRecordWithEmbeddedProto : NSObject
- (nonnull instancetype)init NS_UNAVAILABLE;
+ (nonnull instancetype)new NS_UNAVAILABLE;
- (nonnull instancetype)initWithPerson:(nonnull DJTestPerson *)person NS_DESIGNATED_INITIALIZER;
+ (nonnull instancetype)RecordWithEmbeddedProtoWithPerson:(nonnull DJTestPerson *)person;

@property (nonatomic, readonly, nonnull) DJTestPerson * person;

@end

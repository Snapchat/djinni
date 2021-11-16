// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from derivings.djinni

#import "DBRecordWithDerivings.h"
#import <Foundation/Foundation.h>

@interface DBRecordWithNestedDerivings : NSObject
- (nonnull instancetype)init NS_UNAVAILABLE;
+ (nonnull instancetype)new NS_UNAVAILABLE;
- (nonnull instancetype)initWithKey:(int32_t)key
                                rec:(nonnull DBRecordWithDerivings *)rec NS_DESIGNATED_INITIALIZER;
+ (nonnull instancetype)recordWithNestedDerivingsWithKey:(int32_t)key
                                                     rec:(nonnull DBRecordWithDerivings *)rec;

@property (nonatomic, readonly) int32_t key;

@property (nonatomic, readonly, nonnull) DBRecordWithDerivings * rec;

- (NSComparisonResult)compare:(nonnull DBRecordWithNestedDerivings *)other;

@end

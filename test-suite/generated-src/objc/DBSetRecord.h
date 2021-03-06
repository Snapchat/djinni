// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from set.djinni

#import <Foundation/Foundation.h>

@interface DBSetRecord : NSObject
- (nonnull instancetype)init NS_UNAVAILABLE;
+ (nonnull instancetype)new NS_UNAVAILABLE;
- (nonnull instancetype)initWithSet:(nonnull NSSet<NSString *> *)set
                               iset:(nonnull NSSet<NSNumber *> *)iset NS_DESIGNATED_INITIALIZER;
+ (nonnull instancetype)setRecordWithSet:(nonnull NSSet<NSString *> *)set
                                    iset:(nonnull NSSet<NSNumber *> *)iset;

@property (nonatomic, readonly, nonnull) NSSet<NSString *> * set;

@property (nonatomic, readonly, nonnull) NSSet<NSNumber *> * iset;

@end

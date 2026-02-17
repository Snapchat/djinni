#import "DBSimpleObjectImpl.h"

@implementation DBSimpleObjectImpl {
    int32_t _value;
    NSString* _name;
}

- (instancetype)initWithValue:(int32_t)value name:(NSString*)name {
    if (self = [super init]) {
        _value = value;
        _name = [name copy];
    }
    return self;
}

- (int32_t)getValue {
    return _value;
}

- (nonnull NSString *)getName {
    return _name;
}

@end

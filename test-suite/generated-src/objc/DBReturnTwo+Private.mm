// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from multiple_inheritance.djinni

#import "DBReturnTwo+Private.h"
#import "DBReturnTwo.h"
#import "DJICppWrapperCache+Private.h"
#import "DJIError.h"
#import "DJIMarshal+Private.h"
#include <exception>
#include <stdexcept>
#include <utility>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@interface DBReturnTwo ()

- (id)initWithCpp:(const std::shared_ptr<::testsuite::ReturnTwo>&)cppRef;

@end

@implementation DBReturnTwo {
    ::djinni::CppProxyCache::Handle<std::shared_ptr<::testsuite::ReturnTwo>> _cppRefHandle;
}

- (id)initWithCpp:(const std::shared_ptr<::testsuite::ReturnTwo>&)cppRef
{
    if (self = [super init]) {
        _cppRefHandle.assign(cppRef);
    }
    return self;
}

+ (nonnull DBReturnTwo *)getInstance {
    try {
        auto objcpp_result_ = ::testsuite::ReturnTwo::get_instance();
        return ::djinni_generated::ReturnTwo::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (int8_t)returnTwo {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->return_two();
        return ::djinni::I8::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

namespace djinni_generated {

auto ReturnTwo::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return objc->_cppRefHandle.get();
}

auto ReturnTwo::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return ::djinni::get_cpp_proxy<DBReturnTwo>(cpp);
}

}  // namespace djinni_generated

@end

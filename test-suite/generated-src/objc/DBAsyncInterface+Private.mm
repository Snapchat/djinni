#import "DBAsyncInterface+Private.h"
#import "DBAsyncInterface.h"
#import "DJICppWrapperCache+Private.h"
#import "DJIMarshal+Private.h"
#import "DJIObjcWrapperCache+Private.h"
#include <stdexcept>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

namespace djinni_generated {

class AsyncInterface::ObjcProxy final
: public ::testsuite::AsyncInterface
, private ::djinni::ObjcProxyBase<ObjcType>
{
    friend class ::djinni_generated::AsyncInterface;
public:
    using ObjcProxyBase::ObjcProxyBase;

    djinni::Future<int32_t> get_async_result() {
        @autoreleasepool {
            auto objcpp_result_ = [djinni_private_get_proxied_objc_object() getAsyncResult];
            return ::djinni::FutureAdaptor<::djinni::I32>::toCpp(objcpp_result_);
        }
    }
};

} // namespace djinni_generated

namespace djinni_generated {

auto AsyncInterface::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return ::djinni::get_objc_proxy<ObjcProxy>(objc);
}

auto AsyncInterface::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return dynamic_cast<ObjcProxy&>(*cpp).djinni_private_get_proxied_objc_object();
}

}  // namespace djinni_generated

// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from multiple_inheritance.djinni

#import "DBSecondListener+Private.h"
#import "DBSecondListener.h"
#import "DJIObjcWrapperCache+Private.h"
#include <stdexcept>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

namespace djinni_generated {

class SecondListener::ObjcProxy final
: public ::testsuite::SecondListener
, private ::djinni::ObjcProxyBase<ObjcType>
{
    friend class ::djinni_generated::SecondListener;
public:
    using ObjcProxyBase::ObjcProxyBase;
    void second() override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() second];
        }
    }
};

} // namespace djinni_generated

namespace djinni_generated {

auto SecondListener::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return ::djinni::get_objc_proxy<ObjcProxy>(objc);
}

auto SecondListener::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return dynamic_cast<ObjcProxy&>(*cpp).djinni_private_get_proxied_objc_object();
}

} // namespace djinni_generated

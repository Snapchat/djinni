// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from single_language_interfaces.djinni

#import "DBObjcOnlyListener+Private.h"
#import "DBObjcOnlyListener.h"
#import "DJIObjcWrapperCache+Private.h"
#include <stdexcept>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

namespace djinni_generated {

class ObjcOnlyListener::ObjcProxy final
: public ::testsuite::ObjcOnlyListener
, private ::djinni::ObjcProxyBase<ObjcType>
{
    friend class ::djinni_generated::ObjcOnlyListener;
public:
    using ObjcProxyBase::ObjcProxyBase;
};

} // namespace djinni_generated

namespace djinni_generated {

auto ObjcOnlyListener::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return ::djinni::get_objc_proxy<ObjcProxy>(objc);
}

auto ObjcOnlyListener::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return dynamic_cast<ObjcProxy&>(*cpp).djinni_private_get_proxied_objc_object();
}

} // namespace djinni_generated

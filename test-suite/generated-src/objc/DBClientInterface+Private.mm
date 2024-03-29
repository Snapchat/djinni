// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from client_interface.djinni

#import "DBClientInterface+Private.h"
#import "DBClientInterface.h"
#import "DBClientReturnedRecord+Private.h"
#import "DJIMarshal+Private.h"
#import "DJIObjcWrapperCache+Private.h"
#include <stdexcept>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

namespace djinni_generated {

class ClientInterface::ObjcProxy final
: public ::testsuite::ClientInterface
, private ::djinni::ObjcProxyBase<ObjcType>
{
    friend class ::djinni_generated::ClientInterface;
public:
    using ObjcProxyBase::ObjcProxyBase;
    ::testsuite::ClientReturnedRecord get_record(int64_t c_record_id, const std::string & c_utf8string, const std::experimental::optional<std::string> & c_misc) override
    {
        @autoreleasepool {
            auto objcpp_result_ = [djinni_private_get_proxied_objc_object() getRecord:(::djinni::I64::fromCpp(c_record_id))
                                                                           utf8string:(::djinni::String::fromCpp(c_utf8string))
                                                                                 misc:(::djinni::Optional<std::experimental::optional, ::djinni::String>::fromCpp(c_misc))];
            return ::djinni_generated::ClientReturnedRecord::toCpp(objcpp_result_);
        }
    }
    double identifier_check(const std::vector<uint8_t> & c_data, int32_t c_r, int64_t c_jret) override
    {
        @autoreleasepool {
            auto objcpp_result_ = [djinni_private_get_proxied_objc_object() identifierCheck:(::djinni::Binary::fromCpp(c_data))
                                                                                          r:(::djinni::I32::fromCpp(c_r))
                                                                                       jret:(::djinni::I64::fromCpp(c_jret))];
            return ::djinni::F64::toCpp(objcpp_result_);
        }
    }
    std::string return_str() override
    {
        @autoreleasepool {
            auto objcpp_result_ = [djinni_private_get_proxied_objc_object() returnStr];
            return ::djinni::String::toCpp(objcpp_result_);
        }
    }
    std::string meth_taking_interface(const /*not-null*/ std::shared_ptr<::testsuite::ClientInterface> & c_i) override
    {
        @autoreleasepool {
            auto objcpp_result_ = [djinni_private_get_proxied_objc_object() methTakingInterface:(::djinni_generated::ClientInterface::fromCpp(c_i))];
            return ::djinni::String::toCpp(objcpp_result_);
        }
    }
    std::string meth_taking_optional_interface(const /*nullable*/ std::shared_ptr<::testsuite::ClientInterface> & c_i) override
    {
        @autoreleasepool {
            auto objcpp_result_ = [djinni_private_get_proxied_objc_object() methTakingOptionalInterface:(::djinni::Optional<std::experimental::optional, ::djinni_generated::ClientInterface>::fromCpp(c_i))];
            return ::djinni::String::toCpp(objcpp_result_);
        }
    }
};

} // namespace djinni_generated

namespace djinni_generated {

auto ClientInterface::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return ::djinni::get_objc_proxy<ObjcProxy>(objc);
}

auto ClientInterface::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return dynamic_cast<ObjcProxy&>(*cpp).djinni_private_get_proxied_objc_object();
}

} // namespace djinni_generated

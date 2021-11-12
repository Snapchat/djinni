// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from client_interface.djinni

#include "async_interface.hpp"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@protocol DBAsyncInterface;

namespace djinni_generated {

class AsyncInterface
{
public:
    using CppType = std::shared_ptr<::testsuite::AsyncInterface>;
    using CppOptType = std::shared_ptr<::testsuite::AsyncInterface>;
    using ObjcType = id<DBAsyncInterface>;

    using Boxed = AsyncInterface;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

}  // namespace djinni_generated

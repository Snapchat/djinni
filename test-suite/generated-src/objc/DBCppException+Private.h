// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from exception.djinni

#include "cpp_exception.hpp"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@class DBCppException;

namespace djinni_generated {

class CppException
{
public:
    using CppType = std::shared_ptr<::testsuite::CppException>;
    using CppOptType = std::shared_ptr<::testsuite::CppException>;
    using ObjcType = DBCppException*;

    using Boxed = CppException;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

}  // namespace djinni_generated


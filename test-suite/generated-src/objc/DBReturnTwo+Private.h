// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from multiple_inheritance.djinni

#include "return_two.hpp"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@class DBReturnTwo;

namespace djinni_generated {

class ReturnTwo
{
public:
    using CppType = std::shared_ptr<::testsuite::ReturnTwo>;
    using CppOptType = std::shared_ptr<::testsuite::ReturnTwo>;
    using ObjcType = DBReturnTwo*;

    using Boxed = ReturnTwo;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) {
        if (!cpp) {
            throw std::invalid_argument("ReturnTwo::fromCpp requires a non-null C++ object");
        }
        return fromCppOpt(cpp);
    }

private:
    class ObjcProxy;
};

}  // namespace djinni_generated


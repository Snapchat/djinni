#pragma once

#include "djinni_support.hpp"
#include "../cpp/Future.hpp"

namespace djinni::swift {

// TODO implement
template <class RESULT>
class FutureAdaptor
{
    using CppResType = typename RESULT::CppType;
public:
    using CppType = Future<CppResType>;

    static CppType toCpp(const AnyValue& o) {
        Promise<CppResType> p;
        return p.getFuture();
    }
    static AnyValue fromCpp(const CppType& c) {
        return makeVoidValue();
    }
};

}

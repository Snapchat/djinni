#pragma once

#include "djinni_support.hpp"
#include "../cpp/DataView.hpp"
#include "../cpp/DataRef.hpp"

namespace djinni::swift {

class DataViewAdaptor
{
public:
    using CppType = DataView;
    static CppType toCpp(const AnyValue& s);
    static AnyValue fromCpp(CppType c);
};

class DataRefAdaptor
{
public:
    using CppType = DataRef;
    static CppType toCpp(const AnyValue& s);
    static AnyValue fromCpp(const CppType& c);
};

#if !DATAREF_OBJC
// Without CoreFoundation there is no CFData for Swift's NSData to toll-free bridge to, so a DataRef
// crosses as a heap-held DataRef that the receiver takes over: one from C++ shares its buffer, which
// Swift's Data wraps without copying and releases when deallocated; one from Swift
// (makeDataRefValue) holds a copy of its bytes.
AnyValue makeDataRefValue(const void* bytes, size_t size);
const void* dataRefBytes(const void* holder);
size_t dataRefSize(const void* holder);
void releaseDataRef(const void* holder);
#endif

}

#include "djinni_support.hpp"
#include "DataRef.hpp"

namespace snapchat {
namespace djinni {
struct NativeDataRef {
    using CppType = DataRef;
    using JniType = jobject;

    static CppType toCpp(JNIEnv* jniEnv, JniType data) {
        return DataRef(data);
    }

    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        auto obj = reinterpret_cast<jobject>(c.platformObj());
        return ::djinni::LocalRef<JniType>(jniEnv->NewLocalRef(obj));
    }

    using Boxed = NativeDataRef;
};
}} // namespace snapchat::djinni

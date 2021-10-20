#include "djinni_support.hpp"
#include "Marshal.hpp"

namespace djinni
{

// NOLINTNEXTLINE
static void NativeFutureHandler_nativeHandleResult(JNIEnv* jniEnv, jclass /*unused*/, jlong nativeFunc, jlong nativePromise, jobject jres) {
    auto func = reinterpret_cast<NativeFutureHandlerFunc>(nativeFunc);
    func(jniEnv, nativePromise, jres);
}

static const JNINativeMethod kNativeMethods[] = {{
    const_cast<char*>("nativeHandleResult"),
    const_cast<char*>("(JJLjava/lang/Object;)V"),
    reinterpret_cast<void*>(&NativeFutureHandler_nativeHandleResult),
}};

// NOLINTNEXTLINE
static auto sRegisterMethods =
    ::djinni::JNIMethodLoadAutoRegister("com/snapchat/djinni/NativeFutureHandler", kNativeMethods);

} // namespace djinni

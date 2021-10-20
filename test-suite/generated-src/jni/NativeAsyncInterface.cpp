#include "NativeAsyncInterface.hpp" // my header
#include "Marshal.hpp"

namespace djinni_generated {

NativeAsyncInterface::NativeAsyncInterface() : ::djinni::JniInterface<::testsuite::AsyncInterface, NativeAsyncInterface> () {}

NativeAsyncInterface::~NativeAsyncInterface() = default;

NativeAsyncInterface::JavaProxy::JavaProxy(JniType j) : Handle(::djinni::jniGetThreadEnv(), j) { }

NativeAsyncInterface::JavaProxy::~JavaProxy() = default;

::djinni::Future<int32_t> NativeAsyncInterface::JavaProxy::get_async_result() {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeAsyncInterface>::get();
    auto jret = jniEnv->CallObjectMethod(Handle::get().get(), data.method_getAsyncResult);
    ::djinni::jniExceptionCheck(jniEnv);
    return ::djinni::FutureAdaptor<::djinni::I32>::toCpp(jniEnv, jret);
}


}

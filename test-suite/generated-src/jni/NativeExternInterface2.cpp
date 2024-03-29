// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from yaml-test.djinni

#include "NativeExternInterface2.hpp"  // my header
#include "NativeExternRecordWithDerivings.hpp"
#include "NativeTestHelpers.hpp"

namespace djinni_generated {

NativeExternInterface2::NativeExternInterface2() : ::djinni::JniInterface<::ExternInterface2, NativeExternInterface2>() {}

NativeExternInterface2::~NativeExternInterface2() = default;

NativeExternInterface2::JavaProxy::JavaProxy(JniType j) : Handle(::djinni::jniGetThreadEnv(), j) { }

NativeExternInterface2::JavaProxy::~JavaProxy() = default;

::ExternRecordWithDerivings NativeExternInterface2::JavaProxy::foo(const /*not-null*/ std::shared_ptr<::testsuite::TestHelpers> & c_i) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeExternInterface2>::get();
    auto jret = jniEnv->CallObjectMethod(Handle::get().get(), data.method_foo,
                                         ::djinni::get(::djinni_generated::NativeTestHelpers::fromCpp(jniEnv, c_i)));
    ::djinni::jniExceptionCheck(jniEnv);
    return ::djinni_generated::NativeExternRecordWithDerivings::toCpp(jniEnv, jret);
}

} // namespace djinni_generated

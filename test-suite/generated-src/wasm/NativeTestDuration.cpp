// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from duration.djinni

#include "NativeTestDuration.hpp"  // my header
#include "Duration-wasm.hpp"

namespace djinni_generated {

em::val NativeTestDuration::cppProxyMethods() {
    static const em::val methods = em::val::array(std::vector<std::string> {
    });
    return methods;
}

std::string NativeTestDuration::hoursString(const em::val& w_dt) {
    auto r = ::testsuite::TestDuration::hoursString(::djinni::Duration<::djinni::I32, ::djinni::Duration_h>::toCpp(w_dt));
    return ::djinni::String::fromCpp(r);
}
std::string NativeTestDuration::minutesString(const em::val& w_dt) {
    auto r = ::testsuite::TestDuration::minutesString(::djinni::Duration<::djinni::I32, ::djinni::Duration_min>::toCpp(w_dt));
    return ::djinni::String::fromCpp(r);
}
std::string NativeTestDuration::secondsString(const em::val& w_dt) {
    auto r = ::testsuite::TestDuration::secondsString(::djinni::Duration<::djinni::I32, ::djinni::Duration_s>::toCpp(w_dt));
    return ::djinni::String::fromCpp(r);
}
std::string NativeTestDuration::millisString(const em::val& w_dt) {
    auto r = ::testsuite::TestDuration::millisString(::djinni::Duration<::djinni::I32, ::djinni::Duration_ms>::toCpp(w_dt));
    return ::djinni::String::fromCpp(r);
}
std::string NativeTestDuration::microsString(const em::val& w_dt) {
    auto r = ::testsuite::TestDuration::microsString(::djinni::Duration<::djinni::I32, ::djinni::Duration_us>::toCpp(w_dt));
    return ::djinni::String::fromCpp(r);
}
std::string NativeTestDuration::nanosString(const em::val& w_dt) {
    auto r = ::testsuite::TestDuration::nanosString(::djinni::Duration<::djinni::I32, ::djinni::Duration_ns>::toCpp(w_dt));
    return ::djinni::String::fromCpp(r);
}
em::val NativeTestDuration::hours(int32_t w_count) {
    auto r = ::testsuite::TestDuration::hours(::djinni::I32::toCpp(w_count));
    return ::djinni::Duration<::djinni::I32, ::djinni::Duration_h>::fromCpp(r);
}
em::val NativeTestDuration::minutes(int32_t w_count) {
    auto r = ::testsuite::TestDuration::minutes(::djinni::I32::toCpp(w_count));
    return ::djinni::Duration<::djinni::I32, ::djinni::Duration_min>::fromCpp(r);
}
em::val NativeTestDuration::seconds(int32_t w_count) {
    auto r = ::testsuite::TestDuration::seconds(::djinni::I32::toCpp(w_count));
    return ::djinni::Duration<::djinni::I32, ::djinni::Duration_s>::fromCpp(r);
}
em::val NativeTestDuration::millis(int32_t w_count) {
    auto r = ::testsuite::TestDuration::millis(::djinni::I32::toCpp(w_count));
    return ::djinni::Duration<::djinni::I32, ::djinni::Duration_ms>::fromCpp(r);
}
em::val NativeTestDuration::micros(int32_t w_count) {
    auto r = ::testsuite::TestDuration::micros(::djinni::I32::toCpp(w_count));
    return ::djinni::Duration<::djinni::I32, ::djinni::Duration_us>::fromCpp(r);
}
em::val NativeTestDuration::nanos(int32_t w_count) {
    auto r = ::testsuite::TestDuration::nanos(::djinni::I32::toCpp(w_count));
    return ::djinni::Duration<::djinni::I32, ::djinni::Duration_ns>::fromCpp(r);
}
em::val NativeTestDuration::hoursf(double w_count) {
    auto r = ::testsuite::TestDuration::hoursf(::djinni::F64::toCpp(w_count));
    return ::djinni::Duration<::djinni::F64, ::djinni::Duration_h>::fromCpp(r);
}
em::val NativeTestDuration::minutesf(double w_count) {
    auto r = ::testsuite::TestDuration::minutesf(::djinni::F64::toCpp(w_count));
    return ::djinni::Duration<::djinni::F64, ::djinni::Duration_min>::fromCpp(r);
}
em::val NativeTestDuration::secondsf(double w_count) {
    auto r = ::testsuite::TestDuration::secondsf(::djinni::F64::toCpp(w_count));
    return ::djinni::Duration<::djinni::F64, ::djinni::Duration_s>::fromCpp(r);
}
em::val NativeTestDuration::millisf(double w_count) {
    auto r = ::testsuite::TestDuration::millisf(::djinni::F64::toCpp(w_count));
    return ::djinni::Duration<::djinni::F64, ::djinni::Duration_ms>::fromCpp(r);
}
em::val NativeTestDuration::microsf(double w_count) {
    auto r = ::testsuite::TestDuration::microsf(::djinni::F64::toCpp(w_count));
    return ::djinni::Duration<::djinni::F64, ::djinni::Duration_us>::fromCpp(r);
}
em::val NativeTestDuration::nanosf(double w_count) {
    auto r = ::testsuite::TestDuration::nanosf(::djinni::F64::toCpp(w_count));
    return ::djinni::Duration<::djinni::F64, ::djinni::Duration_ns>::fromCpp(r);
}
em::val NativeTestDuration::box(int64_t w_count) {
    auto r = ::testsuite::TestDuration::box(::djinni::I64::toCpp(w_count));
    return ::djinni::Optional<std::experimental::optional, ::djinni::Duration<::djinni::I64, ::djinni::Duration_s>>::fromCpp(r);
}
int64_t NativeTestDuration::unbox(const em::val& w_dt) {
    auto r = ::testsuite::TestDuration::unbox(::djinni::Optional<std::experimental::optional, ::djinni::Duration<::djinni::I64, ::djinni::Duration_s>>::toCpp(w_dt));
    return ::djinni::I64::fromCpp(r);
}

EMSCRIPTEN_BINDINGS(test_duration) {
    em::class_<::testsuite::TestDuration>("TestDuration")
        .smart_ptr<std::shared_ptr<::testsuite::TestDuration>>("TestDuration")
        .function("nativeDestroy", &NativeTestDuration::nativeDestroy)
        .class_function("hoursString", NativeTestDuration::hoursString)
        .class_function("minutesString", NativeTestDuration::minutesString)
        .class_function("secondsString", NativeTestDuration::secondsString)
        .class_function("millisString", NativeTestDuration::millisString)
        .class_function("microsString", NativeTestDuration::microsString)
        .class_function("nanosString", NativeTestDuration::nanosString)
        .class_function("hours", NativeTestDuration::hours)
        .class_function("minutes", NativeTestDuration::minutes)
        .class_function("seconds", NativeTestDuration::seconds)
        .class_function("millis", NativeTestDuration::millis)
        .class_function("micros", NativeTestDuration::micros)
        .class_function("nanos", NativeTestDuration::nanos)
        .class_function("hoursf", NativeTestDuration::hoursf)
        .class_function("minutesf", NativeTestDuration::minutesf)
        .class_function("secondsf", NativeTestDuration::secondsf)
        .class_function("millisf", NativeTestDuration::millisf)
        .class_function("microsf", NativeTestDuration::microsf)
        .class_function("nanosf", NativeTestDuration::nanosf)
        .class_function("box", NativeTestDuration::box)
        .class_function("unbox", NativeTestDuration::unbox)
        ;
}

}  // namespace djinni_generated

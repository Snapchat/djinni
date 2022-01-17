// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from multiple_inheritance.djinni

#include "NativeListenerCaller.hpp"  // my header
#include "NativeFirstListener.hpp"
#include "NativeSecondListener.hpp"

namespace djinni_generated {

em::val NativeListenerCaller::cppProxyMethods() {
    static const em::val methods = em::val::array(std::vector<std::string> {
        "callFirst",
        "callSecond",
    });
    return methods;
}

em::val NativeListenerCaller::init(const em::val& w_first_l,const em::val& w_second_l) {
    auto r = ::testsuite::ListenerCaller::init(::djinni_generated::NativeFirstListener::toCpp(w_first_l),
         ::djinni_generated::NativeSecondListener::toCpp(w_second_l));
    return ::djinni_generated::NativeListenerCaller::fromCpp(r);
}
void NativeListenerCaller::callFirst(const CppType& self) {
    self->callFirst();
}
void NativeListenerCaller::callSecond(const CppType& self) {
    self->callSecond();
}

namespace {
    EM_JS(void, djinni_init_testsuite_listener_caller, (), {
        'testsuite'.split('.').reduce(function(path, part) {
            if (!(part in path)) { path[part] = {}}; 
            return path[part]}, Module);
        Module.testsuite.ListenerCaller = Module.testsuite_ListenerCaller
    })
}
void NativeListenerCaller::staticInitialize() {
    static std::once_flag initOnce;
    std::call_once(initOnce, djinni_init_testsuite_listener_caller);
}
EMSCRIPTEN_BINDINGS(testsuite_listener_caller) {
    em::class_<::testsuite::ListenerCaller>("testsuite_ListenerCaller")
        .smart_ptr<std::shared_ptr<::testsuite::ListenerCaller>>("testsuite_ListenerCaller")
        .function("nativeDestroy", &NativeListenerCaller::nativeDestroy)
        .class_function("init", NativeListenerCaller::init)
        .function("callFirst", NativeListenerCaller::callFirst)
        .function("callSecond", NativeListenerCaller::callSecond)
        ;
    NativeListenerCaller::staticInitialize();
}

}  // namespace djinni_generated

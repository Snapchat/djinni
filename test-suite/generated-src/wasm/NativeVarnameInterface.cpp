// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from varnames.djinni

#include "NativeVarnameInterface.hpp"  // my header
#include "NativeVarnameRecord.hpp"

namespace djinni_generated {

em::val NativeVarnameInterface::cppProxyMethods() {
    static const em::val methods = em::val::array(std::vector<std::string> {
        "Rmethod",
        "Imethod",
    });
    return methods;
}

em::val NativeVarnameInterface::_rmethod_(const CppType& self, const em::val& w__r_arg_) {
    auto r = self->_rmethod_(::djinni_generated::NativeVarnameRecord::toCpp(w__r_arg_));
    return ::djinni_generated::NativeVarnameRecord::fromCpp(r);
}
em::val NativeVarnameInterface::_imethod_(const CppType& self, const em::val& w__i_arg_) {
    auto r = self->_imethod_(::djinni_generated::NativeVarnameInterface::toCpp(w__i_arg_));
    return ::djinni_generated::NativeVarnameInterface::fromCpp(r);
}

namespace {
    EM_JS(void, djinni_init_testsuite__varname_interface_, (), {
        'testsuite'.split('.').reduce(function(path, part) {
            if (!(part in path)) { path[part] = {}}; 
            return path[part]}, Module);
        Module.testsuite.VarnameInterface = Module.testsuite_VarnameInterface
    })
}
void NativeVarnameInterface::staticInitialize() {
    static std::once_flag initOnce;
    std::call_once(initOnce, djinni_init_testsuite__varname_interface_);
}
EMSCRIPTEN_BINDINGS(testsuite__varname_interface_) {
    em::class_<::testsuite::VarnameInterface>("testsuite_VarnameInterface")
        .smart_ptr<std::shared_ptr<::testsuite::VarnameInterface>>("testsuite_VarnameInterface")
        .function("nativeDestroy", &NativeVarnameInterface::nativeDestroy)
        .function("Rmethod", NativeVarnameInterface::_rmethod_)
        .function("Imethod", NativeVarnameInterface::_imethod_)
        ;
    NativeVarnameInterface::staticInitialize();
}

}  // namespace djinni_generated

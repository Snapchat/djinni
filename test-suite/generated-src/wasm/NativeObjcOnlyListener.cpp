// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from single_language_interfaces.djinni

#include "NativeObjcOnlyListener.hpp"  // my header

namespace djinni_generated {


EMSCRIPTEN_BINDINGS(testsuite_objc_only_listener) {
    ::djinni::DjinniClass_<::testsuite::ObjcOnlyListener>("testsuite_ObjcOnlyListener", "testsuite.ObjcOnlyListener")
        .smart_ptr<std::shared_ptr<::testsuite::ObjcOnlyListener>>("testsuite_ObjcOnlyListener")
        .function("nativeDestroy", &NativeObjcOnlyListener::nativeDestroy)
        ;
}

}  // namespace djinni_generated

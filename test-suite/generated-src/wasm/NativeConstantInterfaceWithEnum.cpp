// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from constant_enum.djinni

#include "NativeConstantInterfaceWithEnum.hpp"  // my header
#include "NativeConstantEnum.hpp"

namespace djinni_generated {

em::val NativeConstantInterfaceWithEnum::cppProxyMethods() {
    static const em::val methods = em::val::array(std::vector<std::string> {
    });
    return methods;
}


EMSCRIPTEN_BINDINGS(testsuite_constant_interface_with_enum) {
    ::djinni::DjinniClass_<::testsuite::ConstantInterfaceWithEnum>("testsuite_ConstantInterfaceWithEnum", "testsuite.ConstantInterfaceWithEnum")
        .smart_ptr<std::shared_ptr<::testsuite::ConstantInterfaceWithEnum>>("testsuite_ConstantInterfaceWithEnum")
        .function("nativeDestroy", &NativeConstantInterfaceWithEnum::nativeDestroy)
        ;
}

namespace {
    EM_JS(void, djinni_init_testsuite_constant_interface_with_enum_consts, (), {
        if (!('testsuite_ConstantInterfaceWithEnum' in Module)) {
            Module.testsuite_ConstantInterfaceWithEnum = {};
        }
        Module.testsuite_ConstantInterfaceWithEnum.CONST_ENUM = Module.testsuite_ConstantEnum.SOME_VALUE;
    })
}
void NativeConstantInterfaceWithEnum::staticInitializeConstants() {
    static std::once_flag initOnce;
    std::call_once(initOnce, [] {
        djinni_init_testsuite_constant_interface_with_enum_consts();
        ::djinni::djinni_register_name_in_ns("testsuite_ConstantInterfaceWithEnum", "testsuite.ConstantInterfaceWithEnum");
    });
}

EMSCRIPTEN_BINDINGS(testsuite_constant_interface_with_enum_consts) {
    NativeConstantEnum::staticInitializeConstants();
    NativeConstantInterfaceWithEnum::staticInitializeConstants();
}

} // namespace djinni_generated

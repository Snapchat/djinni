// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from extended_record.djinni

#include "NativeExtendedRecord.hpp"  // my header

namespace djinni_generated {

auto NativeExtendedRecord::toCpp(const JsType& j) -> CppType {
    return {::djinni::Bool::Boxed::toCpp(j["foo"])};
}
auto NativeExtendedRecord::fromCpp(const CppType& c) -> JsType {
    em::val js = em::val::object();
    js.set("foo", ::djinni::Bool::Boxed::fromCpp(c.foo));
    return js;
}

namespace {
    EM_JS(void, djinni_init_testsuite_extended_record_consts, (), {
        if (!('testsuite_ExtendedRecord' in Module)) {
            Module.testsuite_ExtendedRecord = {};
        }
        Module.testsuite_ExtendedRecord.EXTENDED_RECORD_CONST =  {
            foo: true
        }
        ;
        'testsuite'.split('.').reduce(function(path, part) {
            if (!path.hasOwnProperty(part)) { path[part] = {}}; 
            return path[part]
        }, Module);
        Module.testsuite.ExtendedRecord = Module.testsuite_ExtendedRecord
    })
}
void NativeExtendedRecord::staticInitializeConstants() {
    static std::once_flag initOnce;
    std::call_once(initOnce, djinni_init_testsuite_extended_record_consts);
}

EMSCRIPTEN_BINDINGS(testsuite_extended_record_consts) {
    NativeExtendedRecord::staticInitializeConstants();
}

}  // namespace djinni_generated

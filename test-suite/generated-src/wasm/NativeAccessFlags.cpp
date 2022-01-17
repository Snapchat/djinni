// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from enum_flags.djinni

#include "NativeAccessFlags.hpp"  // my header
#include <mutex>

namespace djinni_generated {

namespace {
    EM_JS(void, djinni_init_testsuite_access_flags, (), {
        Module.testsuite_AccessFlags =  {
            NOBODY : 0,
            OWNER_READ : 1 << 0,
            OWNER_WRITE : 1 << 1,
            OWNER_EXECUTE : 1 << 2,
            GROUP_READ : 1 << 3,
            GROUP_WRITE : 1 << 4,
            GROUP_EXECUTE : 1 << 5,
            SYSTEM_READ : 1 << 6,
            SYSTEM_WRITE : 1 << 7,
            SYSTEM_EXECUTE : 1 << 8,
            EVERYBODY : (1 << 9) - 1,
        }
        ;
        'testsuite'.split('.').reduce(function(path, part) {
            if (!(part in path)) { path[part] = {}}; 
            return path[part]
        }, Module);
        Module.testsuite.AccessFlags = Module.testsuite_AccessFlags
    })
}

void NativeAccessFlags::staticInitializeConstants() {
    static std::once_flag initOnce;
    std::call_once(initOnce, djinni_init_testsuite_access_flags);
}

EMSCRIPTEN_BINDINGS(testsuite_access_flags) {
    NativeAccessFlags::staticInitializeConstants();
}

}  // namespace djinni_generated

// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from array.djinni

#include "NativeTestArray.hpp"  // my header
#include "NativeVec2.hpp"

namespace djinni_generated {

em::val NativeTestArray::cppProxyMethods() {
    static const em::val methods = em::val::array(std::vector<std::string> {
    });
    return methods;
}

em::val NativeTestArray::testStringArray(const em::val& w_a) {
    auto r = ::testsuite::TestArray::testStringArray(::djinni::Array<::djinni::String>::toCpp(w_a));
    return ::djinni::Array<::djinni::String>::fromCpp(r);
}
em::val NativeTestArray::testIntArray(const em::val& w_a) {
    auto r = ::testsuite::TestArray::testIntArray(::djinni::Array<::djinni::I32>::toCpp(w_a));
    return ::djinni::Array<::djinni::I32>::fromCpp(r);
}
em::val NativeTestArray::testRecordArray(const em::val& w_a) {
    auto r = ::testsuite::TestArray::testRecordArray(::djinni::Array<::djinni_generated::NativeVec2>::toCpp(w_a));
    return ::djinni::Array<::djinni_generated::NativeVec2>::fromCpp(r);
}
em::val NativeTestArray::testArrayOfArray(const em::val& w_a) {
    auto r = ::testsuite::TestArray::testArrayOfArray(::djinni::Array<::djinni::Array<::djinni::I32>>::toCpp(w_a));
    return ::djinni::Array<::djinni::Array<::djinni::I32>>::fromCpp(r);
}

namespace {
    EM_JS(void, djinni_init_testsuite_test_array, (), {
        'testsuite'.split('.').reduce(function(path, part) {
            if (!(part in path)) { path[part] = {}}; 
            return path[part]
        }, Module);
        Module.testsuite.TestArray = Module.testsuite_TestArray
    })
}
void NativeTestArray::staticInitialize() {
    static std::once_flag initOnce;
    std::call_once(initOnce, djinni_init_testsuite_test_array);
}
EMSCRIPTEN_BINDINGS(testsuite_test_array) {
    em::class_<::testsuite::TestArray>("testsuite_TestArray")
        .smart_ptr<std::shared_ptr<::testsuite::TestArray>>("testsuite_TestArray")
        .function("nativeDestroy", &NativeTestArray::nativeDestroy)
        .class_function("testStringArray", NativeTestArray::testStringArray)
        .class_function("testIntArray", NativeTestArray::testIntArray)
        .class_function("testRecordArray", NativeTestArray::testRecordArray)
        .class_function("testArrayOfArray", NativeTestArray::testArrayOfArray)
        ;
    NativeTestArray::staticInitialize();
}

}  // namespace djinni_generated

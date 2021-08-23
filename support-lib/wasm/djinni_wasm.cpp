#include "djinni_wasm.hpp"

namespace djinni {

Binary::CppType Binary::toCpp(const JsType& j) {
    return em::convertJSArrayToNumberVector<uint8_t>(j);
}
Binary::JsType Binary::fromCpp(const CppType& c) {
    em::val memoryView{ em::typed_memory_view(c.size(), c.data()) };
    return memoryView.call<em::val>("slice", 0);
}

Date::CppType Date::toCpp(const JsType& j) {
    auto milliesSinceEpoch = std::chrono::milliseconds(static_cast<int64_t>(j.call<em::val>("getTime").as<double>()));
    return CppType(milliesSinceEpoch);
}
Date::JsType Date::fromCpp(const CppType& c) {
    auto milliesSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(c.time_since_epoch());
    static em::val dateType = em::val::global("Date");
    return dateType.new_(static_cast<double>(milliesSinceEpoch.count()));
}

JsProxyId nextId = 0;
std::unordered_map<JsProxyId, std::weak_ptr<JsProxyBase>> jsProxyCache;
std::unordered_map<void*, em::val> cppProxyCache;
std::mutex jsProxyCacheMutex;
std::mutex cppProxyCacheMutex;

JsProxyBase::JsProxyBase(const em::val& v) : _js(v), _id(_js["_djinni_js_proxy_id"].as<JsProxyId>()) {
}

JsProxyBase::~JsProxyBase() {
    std::lock_guard lk(jsProxyCacheMutex);
    jsProxyCache.erase(_id);
}

const em::val& JsProxyBase::_jsRef() const {
    return _js;
}

em::val getCppProxyFinalizerRegistry() {
    EM_ASM(
        console.log("create cppProxyFinalizerRegistry");
        Module.cppProxyFinalizerRegistry = new FinalizationRegistry(nativeRef => {
            console.log("finalizing cpp object");
            nativeRef.nativeDestroy();
            nativeRef.delete();
        });
    );
    return em::val::module_property("cppProxyFinalizerRegistry");
}

em::val getCppProxyClass() {
    static std::once_flag defineProxyClassOnce;
    std::call_once(defineProxyClassOnce, []{
        EM_ASM(
            console.log("define cpp proxy class");
            class DjinniCppProxy {
                constructor(nativeRef, methods) {
                    console.log('new cpp proxy');
                    this._djinni_native_ref = nativeRef;
                    let self = this;
                    methods.forEach(function(method) {
                            self[method] = function(...args) {
                                return nativeRef[method](...args);
                            }
                        });
                }
            }
            Module.DjinniCppProxy = DjinniCppProxy;
        );
    });
    static auto inst  = em::val::module_property("DjinniCppProxy");
    return inst;
}

}

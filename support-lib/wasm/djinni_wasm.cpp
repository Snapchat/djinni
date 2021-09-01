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
    auto nanosSinceEpoch = std::chrono::nanoseconds(static_cast<int64_t>(j.call<em::val>("getTime").as<double>() * 1000000));
    return CppType(std::chrono::duration_cast<std::chrono::system_clock::duration>(nanosSinceEpoch));
}
Date::JsType Date::fromCpp(const CppType& c) {
    auto nanosSinceEpoch = std::chrono::duration_cast<std::chrono::nanoseconds>(c.time_since_epoch());
    static em::val dateType = em::val::global("Date");
    return dateType.new_(static_cast<double>(nanosSinceEpoch.count()) / 1000000.0);
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

EM_JS(void, djinni_init, (), {
    if (typeof Module.cppProxyFinalizerRegistry == 'undefined') {
        console.log("create cppProxyFinalizerRegistry");
        Module.cppProxyFinalizerRegistry = new FinalizationRegistry(nativeRef => {
            console.log("finalizing cpp object");
            nativeRef.nativeDestroy();
            nativeRef.delete();
        });
    }
    if (typeof Module.DjinniCppProxy == 'undefined') {
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
    }
});

static em::val getJsHelper(const char* name) {
    static std::once_flag djinniInitOnce;
    std::call_once(djinniInitOnce, djinni_init);
    return em::val::module_property(name);
}

em::val getCppProxyFinalizerRegistry() {
    static auto inst  = getJsHelper("cppProxyFinalizerRegistry");
    return inst;
}

em::val getCppProxyClass() {
    static auto inst  = getJsHelper("DjinniCppProxy");
    return inst;
}

}

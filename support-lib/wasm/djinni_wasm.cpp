/**
  * Copyright 2021 Snap, Inc.
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *    http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

#include "djinni_wasm.hpp"

namespace djinni {

Binary::CppType Binary::toCpp(const JsType& j) {
    return PrimitiveArray<Primitive<uint8_t>, Binary>::toCpp(j);
}
Binary::JsType Binary::fromCpp(const CppType& c) {
    static em::val arrayClass = em::val::global("Uint8Array");
    return PrimitiveArray<Primitive<uint8_t>, Binary>::fromCpp(c);
}
em::val Binary::getArrayClass() {
    static em::val arrayClass = em::val::global("Uint8Array");
    return arrayClass;
}

Date::CppType Date::toCpp(const JsType& j) {
    auto nanosSinceEpoch = std::chrono::nanoseconds(static_cast<int64_t>(j.call<em::val>("getTime").as<double>() * 1'000'000));
    return CppType(std::chrono::duration_cast<std::chrono::system_clock::duration>(nanosSinceEpoch));
}
Date::JsType Date::fromCpp(const CppType& c) {
    auto nanosSinceEpoch = std::chrono::duration_cast<std::chrono::nanoseconds>(c.time_since_epoch());
    static em::val dateType = em::val::global("Date");
    return dateType.new_(static_cast<double>(nanosSinceEpoch.count()) / 1'000'000.0);
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

void JsProxyBase::checkError(const em::val& v) {
    if (v.instanceof(em::val::global("Error"))) {
         // The stack property is non-standard, but well supported in browsers
         // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Error/Stack#browser_compatibility
         // It also seems to include the name and message properties, so no need to access them separately
         //
         // >>> function doThrow() { throw new Error("foo")}
         // >>> try { doThrow() } catch (e) { console.log(e.stack) }
         // Error: foo
         //     at doThrow (<anonymous>:1:25)
         //     at <anonymous>:1:7
         // >>> try { doThrow() } catch (e) { console.log(e.name) }
         // Error
         // >>> try { doThrow() } catch (e) { console.log(e.message) }
         // foo
        throw JsException(v["stack"].as<std::string>());
    }
}

em::val getCppProxyFinalizerRegistry() {
    static auto inst  = em::val::module_property("cppProxyFinalizerRegistry");
    return inst;
}

em::val getCppProxyClass() {
    static auto inst  = em::val::module_property("DjinniCppProxy");
    return inst;
}

em::val getWasmMemoryBuffer() {
    static auto memoryBuffer = em::val::module_property("HEAPU32")["buffer"];
    return memoryBuffer;
}

em::val DataObject::createJsObject() {
    static auto finalizerRegistry = em::val::module_property("directBufferFinalizerRegistry");
    static auto uint8ArrayClass = em::val::global("Uint8Array");
    em::val jsObj = uint8ArrayClass.new_(getWasmMemoryBuffer(), addr(), size());
    finalizerRegistry.call<void>("register", jsObj, reinterpret_cast<unsigned>(this));
    return jsObj;
}

static em::val allocateWasmBuffer(unsigned size) {
    auto* dbuf = new GenericBuffer<std::vector<uint8_t>>(size);
    return dbuf->createJsObject();
}

extern "C" EMSCRIPTEN_KEEPALIVE
void releaseWasmBuffer(unsigned addr) {
    delete reinterpret_cast<DataObject*>(addr);
}

static std::string getExceptionMessage(int eptr)
{
    return reinterpret_cast<std::exception*>(eptr)->what();
}

EM_JS(void, djinni_init_wasm, (), {
        console.log("djinni_init_wasm");
        Module.cppProxyFinalizerRegistry = new FinalizationRegistry(nativeRef => {
            // console.log("finalizing cpp object @" + nativeRef);
            nativeRef.nativeDestroy();
            nativeRef.delete();
        });

        Module.directBufferFinalizerRegistry = new FinalizationRegistry(addr => {
            Module._releaseWasmBuffer(addr);
        });

        class DjinniCppProxy {
            constructor(nativeRef, methods) {
                // console.log('new cpp proxy');
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

        class DjinniJsPromiseBuilder {
            constructor(cppHandlerPtr) {
                this.promise = new Promise((resolveFunc, rejectFunc) => {
                        Module.initCppResolveHandler(cppHandlerPtr, resolveFunc, rejectFunc);
                    });
            }
        }
        Module.DjinniJsPromiseBuilder = DjinniJsPromiseBuilder;

        Module.makeNativePromiseResolver = function(func, pNativePromise) {
            return function(res) {
                Module.resolveNativePromise(func, pNativePromise, res, null);
            };
        };
        Module.makeNativePromiseRejecter = function(func, pNativePromise) {
            return function(err) {
                Module.resolveNativePromise(func, pNativePromise, null, err);
            };
        };

        Module.writeNativeMemory = function(src, nativePtr) {
            var srcByteView = new Uint8Array(src.buffer, src.byteOffset, src.byteLength);
            Module.HEAPU8.set(srcByteView, nativePtr);
        };
        Module.readNativeMemory = function(cls, nativePtr, nativeSize) {
            return new cls(Module.HEAPU8.buffer.slice(nativePtr, nativePtr + nativeSize));
        };

        Module.protobuf = {};
        Module.registerProtobufLib = function(name, proto) {
            Module.protobuf[name] = proto;
        };

        Module.callJsProxyMethod = function(obj, method, ...args) {
            try {
                return obj[method].apply(obj, args);
            } catch (e) {
                return e;
            }
        };
});

EMSCRIPTEN_BINDINGS(djinni_wasm) {
    djinni_init_wasm();    
    em::function("allocateWasmBuffer", &allocateWasmBuffer);
    em::function("getExceptionMessage", &getExceptionMessage);
    em::function("initCppResolveHandler", &CppResolveHandlerBase::initInstance);
    em::function("resolveNativePromise", &CppResolveHandlerBase::resolveNativePromise);
}

}

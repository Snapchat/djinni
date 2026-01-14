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

#pragma once

#include "djinni_support.hpp"
#include "Marshal.hpp"
#include <functional>

namespace djinni {

struct SupplierJniInfo {
    const GlobalRef<jclass> clazz { jniFindClass("java/util/function/Supplier") };
    const jmethodID method_get { jniGetMethodID(clazz.get(), "get", "()Ljava/lang/Object;") };
};

// Forward declaration of type-erased holder
struct LazyFunctionHolder {
    virtual ~LazyFunctionHolder() = default;
    virtual jobject call(JNIEnv* env) = 0;
};

// Template implementation for specific types
template<typename T>
struct TypedLazyFunctionHolder : LazyFunctionHolder {
    using CppType = typename T::CppType;
    using CppLazyType = std::function<CppType()>;
    
    CppLazyType func;
    
    TypedLazyFunctionHolder(CppLazyType f) : func(std::move(f)) {}
    
    jobject call(JNIEnv* env) override {
        auto result = func();
        auto jresult = T::Boxed::fromCpp(env, result);
        return jresult.release();
    }
};

struct NativeLazyHandlerJniInfo {
    const GlobalRef<jclass> clazz { jniFindClass("com/snapchat/djinni/NativeLazyHandler") };
    const jmethodID constructor { jniGetMethodID(clazz.get(), "<init>", "(J)V") };
};

template <class T>
class LazyAdaptor {
    using CppType = std::function<typename T::CppType()>;
    using CppResType = typename T::CppType;
    using JniResType = typename T::Boxed::JniType;

public:
    using JniType = jobject;
    using Boxed = LazyAdaptor;

    static CppType toCpp(JNIEnv* jniEnv, JniType j) {
        // Capture the Java Supplier object in a GlobalRef wrapped in shared_ptr
        auto supplier = std::make_shared<GlobalRef<jobject>>(jniEnv, j);
        
        return [supplier]() -> CppResType {
            JNIEnv* jniEnv = jniGetThreadEnv();
            const auto& supplierJniInfo = JniClass<SupplierJniInfo>::get();
            auto result = LocalRef<jobject>(jniEnv, jniEnv->CallObjectMethod(
                supplier->get(), 
                supplierJniInfo.method_get
            ));
            jniExceptionCheck(jniEnv);
            return T::Boxed::toCpp(jniEnv, static_cast<JniResType>(result.get()));
        };
    }

    static LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) {
        // Create a type-erased holder for the C++ function
        // The implementation is in Lazy_jni.cpp
        auto* holder = new TypedLazyFunctionHolder<T>(c);
        
        // Create a Java NativeLazyHandler that calls back to C++
        const auto& nativeLazyHandlerJniInfo = JniClass<NativeLazyHandlerJniInfo>::get();
        
        auto handler = LocalRef<jobject>(jniEnv, jniEnv->NewObject(
            nativeLazyHandlerJniInfo.clazz.get(),
            nativeLazyHandlerJniInfo.constructor,
            reinterpret_cast<jlong>(holder)
        ));
        jniExceptionCheck(jniEnv);
        
        return handler;
    }
};

} // namespace djinni

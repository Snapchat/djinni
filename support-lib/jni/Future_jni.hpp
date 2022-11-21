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
#include "../cpp/Future.hpp"

namespace djinni {

struct PromiseJniInfo {
    const GlobalRef<jclass> clazz { jniFindClass("com/snapchat/djinni/Promise") };
    const jmethodID constructor { jniGetMethodID(clazz.get(), "<init>", "()V") };
    const jmethodID method_get_future { jniGetMethodID(clazz.get(), "getFuture", "()Lcom/snapchat/djinni/Future;") };
    const jmethodID method_set_value { jniGetMethodID(clazz.get(), "setValue", "(Ljava/lang/Object;)V") };
    const jmethodID method_set_exception { jniGetMethodID(clazz.get(), "setException", "(Ljava/lang/Throwable;)V") };
};

struct FutureJniInfo {
    const GlobalRef<jclass> clazz { jniFindClass("com/snapchat/djinni/Future") };
    const jmethodID method_then { jniGetMethodID(clazz.get(), "then", "(Lcom/snapchat/djinni/Future$FutureHandler;)Lcom/snapchat/djinni/Future;") };
};

struct NativeFutureHandlerJniInfo {
    const GlobalRef<jclass> clazz { jniFindClass("com/snapchat/djinni/NativeFutureHandler") };
    const jmethodID constructor { jniGetMethodID(clazz.get(), "<init>", "(JJ)V") };
};

struct RuntimeExceptionJniInfo {
    const GlobalRef<jclass> clazz { jniFindClass("java/lang/RuntimeException") };
    const jmethodID constructor { jniGetMethodID(clazz.get(), "<init>", "(Ljava/lang/String;)V") };
};

struct ThrowableJniInfo {
    const GlobalRef<jclass> clazz { jniFindClass("java/lang/Throwable") };
    const jmethodID method_get_message { jniGetMethodID(clazz.get(), "getMessage", "()Ljava/lang/String;") };
};

using NativeFutureHandlerFunc = void (*)(JNIEnv* jniEnv, jlong nativePromise, jobject jres, jthrowable jex);

template<typename T>
struct SetResult {
    static void setCppResult(JNIEnv* jniEnv, Promise<typename T::CppType>& promise, jobject jres) {
        promise.setValue(T::Boxed::toCpp(jniEnv, static_cast<typename T::Boxed::JniType>(jres)));
    }
    static void setJavaResult(JNIEnv* jniEnv, const PromiseJniInfo& promiseJniInfo, jobject promise, Future<typename T::CppType>& cppFuture) {
        jniEnv->CallVoidMethod(promise, promiseJniInfo.method_set_value, T::Boxed::fromCpp(jniEnv, cppFuture.get()).get());
    }
};

template<>
struct SetResult<Void> {
    static void setCppResult(JNIEnv* jniEnv, Promise<void>& promise, jobject jres) {
        promise.setValue();
    }
    static void setJavaResult(JNIEnv* jniEnv, const PromiseJniInfo& promiseJniInfo, jobject promise, Future<void>& cppFuture) {
        jniEnv->CallVoidMethod(promise, promiseJniInfo.method_set_value, nullptr);
    }
};

template <class RESULT>
class FutureAdaptor
{
    using CppResType = typename RESULT::CppType;

    struct PromiseHolder {
        GlobalRef<jobject> promise;

        void set(JNIEnv * env, jobject localRef) {
            promise = GlobalRef<jobject>(env, localRef);
        }
        auto get() const { return promise.get(); }
    };
public:
    using CppType = Future<CppResType>;
    using JniType = jobject;

    using Boxed = FutureAdaptor;

    static CppType toCpp(JNIEnv* jniEnv, JniType j)
    {
        using NativePromiseType = Promise<CppResType>;
            
        auto p = std::make_unique<NativePromiseType>();
        auto f = p->getFuture();

        NativeFutureHandlerFunc FutureHandler = [] (JNIEnv* jniEnv, jlong nativePromise, jobject jres, jthrowable jex) {
            std::unique_ptr<NativePromiseType> promise {
                reinterpret_cast<NativePromiseType*>(nativePromise)
            };
            if (jex == nullptr) {
                SetResult<RESULT>::setCppResult(jniEnv, *promise, static_cast<typename RESULT::Boxed::JniType>(jres));
            } else {
                const auto& throwableJniInfo = JniClass<ThrowableJniInfo>::get();
                LocalRef<jstring> jmsg(jniEnv, static_cast<jstring>(jniEnv->CallObjectMethod(jex,
                                                                                             throwableJniInfo.method_get_message,
                                                                                             jex)));
                std::string msg = jniUTF8FromString(jniEnv, jmsg.get());
                promise->setException(std::runtime_error(msg));
            }
        };

        const auto& nativeFutureHandlerJniInfo = JniClass<NativeFutureHandlerJniInfo>::get();
        auto handler = LocalRef<jobject>(jniEnv, jniEnv->NewObject(nativeFutureHandlerJniInfo.clazz.get(),
                                                                   nativeFutureHandlerJniInfo.constructor,
                                                                   reinterpret_cast<jlong>(FutureHandler),
                                                                   reinterpret_cast<jlong>(p.release())));
        jniExceptionCheck(jniEnv);
        const auto& futureJniInfo = JniClass<FutureJniInfo>::get();
        jniEnv->CallObjectMethod(j, futureJniInfo.method_then, handler.get());
        jniExceptionCheck(jniEnv);
        return f;
    }

    static LocalRef<JniType> fromCpp(JNIEnv* jniEnv, CppType c)
    {
        const auto& promiseJniInfo = JniClass<PromiseJniInfo>::get();

        auto promise = std::make_shared<PromiseHolder>();
        promise->set(jniEnv, jniEnv->NewObject(promiseJniInfo.clazz.get(), promiseJniInfo.constructor));
        auto future = LocalRef<jobject>(jniEnv, jniEnv->CallObjectMethod(promise->get(), promiseJniInfo.method_get_future));
        jniExceptionCheck(jniEnv);

        c.then([promise, &promiseJniInfo] (Future<CppResType> cppFuture) {
            JNIEnv* jniEnv = jniGetThreadEnv();
            try {
                // auto res = cppFuture.get();
                // jniEnv->CallVoidMethod(promise->get(), promiseJniInfo.method_set_value, RESULT::Boxed::fromCpp(jniEnv, res).get());
                SetResult<RESULT>::setJavaResult(jniEnv, promiseJniInfo, promise->get(), cppFuture);
            } catch (const std::exception& e) {
                // create a java exception object
                const auto& exceptionJniInfo = JniClass<RuntimeExceptionJniInfo>::get();
                LocalRef<jobject> jex(jniEnv, jniEnv->NewObject(exceptionJniInfo.clazz.get(), exceptionJniInfo.constructor, String::fromCpp(jniEnv, e.what()).get()));
                // call setException()
                jniEnv->CallVoidMethod(promise->get(), promiseJniInfo.method_set_exception, jex.get());
            }
            jniExceptionCheck(jniEnv);
        });
            
        return future;
    }
};

} // namespace djinni

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

namespace snapchat {
namespace djinni {
struct PromiseJniInfo {
    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/snapchat/djinni/Promise") };
    const jmethodID constructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "()V") };
    const jmethodID method_get_future { ::djinni::jniGetMethodID(clazz.get(), "getFuture", "()Lcom/snapchat/djinni/Future;") };
    const jmethodID method_set_value { ::djinni::jniGetMethodID(clazz.get(), "setValue", "(Ljava/lang/Object;)V") };
    const jmethodID method_set_exception { ::djinni::jniGetMethodID(clazz.get(), "setException", "(Ljava/lang/Throwable;)V") };
};

struct FutureJniInfo {
    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/snapchat/djinni/Future") };
    const jmethodID method_then { ::djinni::jniGetMethodID(clazz.get(), "then", "(Lcom/snapchat/djinni/Future$FutureHandler;)Lcom/snapchat/djinni/Future;") };
};

struct NativeFutureHandlerJniInfo {
    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/snapchat/djinni/NativeFutureHandler") };
    const jmethodID constructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(JJ)V") };
};

struct RuntimeExceptionJniInfo {
    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("java/lang/RuntimeException") };
    const jmethodID constructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(Ljava/lang/String;)V") };
};

struct ThrowableJniInfo {
    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("java/lang/Throwable") };
    const jmethodID method_get_message { ::djinni::jniGetMethodID(clazz.get(), "getMessage", "()Ljava/lang/String;") };
};

using NativeFutureHandlerFunc = void (*)(JNIEnv* jniEnv, jlong nativePromise, jobject jres, jthrowable jex);

template <class RESULT>
class FutureAdaptor
{
    using CppResType = typename RESULT::CppType;

    struct PromiseHolder {
        ::djinni::GlobalRef<jobject> promise;

        void set(JNIEnv * env, jobject localRef) {
            promise = ::djinni::GlobalRef<jobject>(env, localRef);
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
                promise->setValue(RESULT::Boxed::toCpp(jniEnv, jres));
            } else {
                const auto& throwableJniInfo = ::djinni::JniClass<ThrowableJniInfo>::get();
                ::djinni::LocalRef<jstring> jmsg(jniEnv, static_cast<jstring>(jniEnv->CallObjectMethod(jex,
                                                                                             throwableJniInfo.method_get_message,
                                                                                             jex)));
                std::string msg = ::djinni::jniUTF8FromString(jniEnv, jmsg.get());
                try {
                    throw std::runtime_error(msg);
                } catch (std::exception&) {
                    promise->setException(std::current_exception());
                }
            }
        };

        const auto& nativeFutureHandlerJniInfo = ::djinni::JniClass<NativeFutureHandlerJniInfo>::get();
        auto handler = ::djinni::LocalRef<jobject>(jniEnv, jniEnv->NewObject(nativeFutureHandlerJniInfo.clazz.get(),
                                                                   nativeFutureHandlerJniInfo.constructor,
                                                                   reinterpret_cast<jlong>(FutureHandler),
                                                                   reinterpret_cast<jlong>(p.release())));
        ::djinni::jniExceptionCheck(jniEnv);
        const auto& futureJniInfo = ::djinni::JniClass<FutureJniInfo>::get();
        jniEnv->CallObjectMethod(j, futureJniInfo.method_then, handler.get());
        ::djinni::jniExceptionCheck(jniEnv);
        return f;
    }

    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, CppType c)
    {
        const auto& promiseJniInfo = ::djinni::JniClass<PromiseJniInfo>::get();

        auto promise = std::make_shared<PromiseHolder>();
        promise->set(jniEnv, jniEnv->NewObject(promiseJniInfo.clazz.get(), promiseJniInfo.constructor));
        auto future = ::djinni::LocalRef<jobject>(jniEnv, jniEnv->CallObjectMethod(promise->get(), promiseJniInfo.method_get_future));
        ::djinni::jniExceptionCheck(jniEnv);
                        
        c.then([promise, &promiseJniInfo] (Future<CppResType> cppFuture) {
            JNIEnv* jniEnv = ::djinni::jniGetThreadEnv();
            try {
                auto res = cppFuture.get();
                jniEnv->CallVoidMethod(promise->get(), promiseJniInfo.method_set_value, RESULT::Boxed::fromCpp(jniEnv, res).get());
            } catch (std::exception& e) {
                // create a java exception object
                const auto& exceptionJniInfo = ::djinni::JniClass<RuntimeExceptionJniInfo>::get();
                ::djinni::LocalRef<jobject> jex(jniEnv, jniEnv->NewObject(exceptionJniInfo.clazz.get(), exceptionJniInfo.constructor, ::djinni::String::fromCpp(jniEnv, e.what()).get()));
                // call setException()
                jniEnv->CallVoidMethod(promise->get(), promiseJniInfo.method_set_exception, jex.get());
            }
            ::djinni::jniExceptionCheck(jniEnv);
        });
            
        return future;
    }
};
}} // namespace snapchat::djinni

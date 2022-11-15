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

struct CompletbleFutureJniInfo {
    const GlobalRef<jclass> clazz { jniFindClass("java/util/concurrent/CompletableFuture") };
    const jmethodID constructor { jniGetMethodID(clazz.get(), "<init>", "()V") };
    const jmethodID method_complete { jniGetMethodID(clazz.get(), "complete", "(Ljava/lang/Object;)Z") };
    const jmethodID method_complete_exception { jniGetMethodID(clazz.get(), "completeExceptionally", "(Ljava/lang/Throwable;)Z") };
    const jmethodID method_when_complete { jniGetMethodID(clazz.get(), "whenComplete", "(Ljava/util/function/BiConsumer;)Ljava/util/concurrent/CompletableFuture;") };
};

struct NativeFutureHandlerJniInfo {
    const GlobalRef<jclass> clazz { jniFindClass("com/snapchat/djinni/NativeFutureHandler") };
    const jmethodID constructor { jniGetMethodID(clazz.get(), "<init>", "(JJ)V") };
};

struct ExecutionExceptionJniInfo {
    const GlobalRef<jclass> clazz { jniFindClass("java/util/concurrent/ExecutionException") };
    const jmethodID constructor { jniGetMethodID(clazz.get(), "<init>", "(Ljava/lang/String;)V") };
};

struct ThrowableJniInfo {
    const GlobalRef<jclass> clazz { jniFindClass("java/lang/Throwable") };
    const jmethodID method_get_message { jniGetMethodID(clazz.get(), "getMessage", "()Ljava/lang/String;") };
};

using NativeFutureHandlerFunc = void (*)(JNIEnv* jniEnv, jlong nativePromise, jobject jres, jthrowable jex);

template <class RESULT>
class FutureAdaptor
{
    using CppResType = typename RESULT::CppType;

    struct FutureHolder {
        GlobalRef<jobject> javaFuture;
        FutureHolder(JNIEnv * env, jobject localRef) {
            javaFuture = GlobalRef<jobject>(env, localRef);
        }
        auto get() const { return javaFuture.get(); }
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
                promise->setValue(RESULT::Boxed::toCpp(jniEnv, static_cast<typename RESULT::Boxed::JniType>(jres)));
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
        const auto& futureJniInfo = JniClass<CompletbleFutureJniInfo>::get();
        jniEnv->CallObjectMethod(j, futureJniInfo.method_when_complete, handler.get());
        jniExceptionCheck(jniEnv);
        return f;
    }

    static LocalRef<JniType> fromCpp(JNIEnv* jniEnv, CppType c)
    {
        const auto& futureJniInfo = JniClass<CompletbleFutureJniInfo>::get();

        auto javaFuture = LocalRef<jobject>(jniEnv->NewObject(futureJniInfo.clazz.get(), futureJniInfo.constructor));
        auto holder = std::make_shared<FutureHolder>(jniEnv, javaFuture);
        jniExceptionCheck(jniEnv);

        c.then([holder, &futureJniInfo] (Future<CppResType> cppFuture) {
            JNIEnv* jniEnv = jniGetThreadEnv();
            try {
                auto res = cppFuture.get();
                jniEnv->CallVoidMethod(holder->get(), futureJniInfo.method_complete, RESULT::Boxed::fromCpp(jniEnv, res).get());
            } catch (const std::exception& e) {
                // create a java exception object
                const auto& exceptionJniInfo = JniClass<ExecutionExceptionJniInfo>::get();
                LocalRef<jobject> jex(jniEnv, jniEnv->NewObject(exceptionJniInfo.clazz.get(), exceptionJniInfo.constructor, String::fromCpp(jniEnv, e.what()).get()));
                // call setException()
                jniEnv->CallVoidMethod(holder->get(), futureJniInfo.method_complete_exception, jex.get());
            }
            jniExceptionCheck(jniEnv);
        });
            
        return javaFuture;
    }
};
} // namespace djinni

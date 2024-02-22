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
#include "expected.hpp"

namespace djinni {

struct OutcomeJniInfo
{
    const GlobalRef<jclass> clazz { jniFindClass("com/snapchat/djinni/Outcome") };
    const jmethodID method_from_result { jniGetStaticMethodID(clazz.get(), "fromResult", "(Ljava/lang/Object;)Lcom/snapchat/djinni/Outcome;") };
    const jmethodID method_from_error { jniGetStaticMethodID(clazz.get(), "fromError", "(Ljava/lang/Object;)Lcom/snapchat/djinni/Outcome;") };
    const jmethodID method_result_or { jniGetMethodID(clazz.get(), "resultOr", "(Ljava/lang/Object;)Ljava/lang/Object;") };
    const jmethodID method_error_or_null { jniGetMethodID(clazz.get(), "errorOrNull", "()Ljava/lang/Object;") };
};

template <class RESULT, class ERROR>
class Outcome
{
    using ResultCppType = typename RESULT::CppType;
    using ErrorCppType = typename ERROR::CppType;
    using ResultJniType = typename RESULT::Boxed::JniType;
    using ErrorJniType = typename ERROR::Boxed::JniType;
public:
    using CppType = expected<ResultCppType, ErrorCppType>;
    using JniType = jobject;

    using Boxed = Outcome;

    static CppType toCpp(JNIEnv* jniEnv, JniType j)
    {
        const auto& outcomeJniInfo = JniClass<OutcomeJniInfo>::get();
        auto r = LocalRef<jobject>(jniEnv, jniEnv->CallObjectMethod(j, outcomeJniInfo.method_result_or, nullptr));
        jniExceptionCheck(jniEnv);
        if (r.get() != nullptr) {
            return RESULT::Boxed::toCpp(jniEnv, reinterpret_cast<typename RESULT::Boxed::JniType>(r.get()));
        } else {
            auto e = LocalRef<jobject>(jniEnv, jniEnv->CallObjectMethod(j, outcomeJniInfo.method_error_or_null));
            jniExceptionCheck(jniEnv);
            // if result is not present then error must be present, we can skip the present check
            return make_unexpected(ERROR::Boxed::toCpp(jniEnv, reinterpret_cast<typename ERROR::Boxed::JniType>(e.get())));
        }
    }

    static LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c)
    {
        const auto& outcomeJniInfo = JniClass<OutcomeJniInfo>::get();
        auto convertResult = [&](const ResultCppType& result){
            auto r = RESULT::Boxed::fromCpp(jniEnv, result);
            auto o = LocalRef<jobject>(jniEnv, jniEnv->CallStaticObjectMethod(outcomeJniInfo.clazz.get(), outcomeJniInfo.method_from_result, r.get()));
            jniExceptionCheck(jniEnv);
            return o;
        };
        auto convertError = [&](const ErrorCppType& error){
            auto e = ERROR::Boxed::fromCpp(jniEnv, error);
            auto o = LocalRef<jobject>(jniEnv, jniEnv->CallStaticObjectMethod(outcomeJniInfo.clazz.get(), outcomeJniInfo.method_from_error, e.get()));
            jniExceptionCheck(jniEnv);
            return o;
        };
        return c.has_value() ? convertResult(c.value()) : convertError(c.error());
    }
};

} // namespace djinni

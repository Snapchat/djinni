/**
  * Copyright 2024 Snap, Inc.
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

#include "djinni_composer.hpp"
#include "../cpp/Future.hpp"
#include "composer_core/cpp/Utils/ResolvablePromise.hpp"

namespace djinni {

template <class RESULT>
class FutureAdaptor
{
    using CppResType = typename RESULT::CppType;

public:
    using CppType = Future<CppResType>;
    using ComposerType = Composer::Value;

    using Boxed = FutureAdaptor;

    using NativePromiseType = Promise<CppResType>;

    static CppType toCpp(ComposerType o)
    {
        auto composerPromise = castOrNull<Composer::Promise>(o.getComposerObject());
        auto cppPromise = Composer::makeShared<Promise<CppResType>>();
        auto cppFuture = cppPromise->getFuture();
        composerPromise->onComplete([cppPromise] (const Composer::Result<Composer::Value>& result) {
            if (result.success()) {
                if constexpr(std::is_same_v<Void, RESULT>) {
                    cppPromise->setValue();
                } else {
                    cppPromise->setValue(RESULT::toCpp(result.value()));
                }
            } else {
                cppPromise->setException(std::runtime_error(result.error().toString().c_str()));
            }
        });
        return cppFuture;
    }

    static ComposerType fromCpp(CppType c)
    {
        auto composerPromise = Composer::makeShared<Composer::ResolvablePromise>();
        c.then([composerPromise] (Future<CppResType> f) {
            try {
                if constexpr(std::is_same_v<Void, RESULT>) {
                    composerPromise->fulfill(Composer::Result<Composer::Value>(Composer::Value::undefined()));
                } else {
                    composerPromise->fulfill(Composer::Result<Composer::Value>(RESULT::fromCpp(f.get())));
                }
            } catch (const std::exception& e) {
                composerPromise->fulfill({Composer::Result<Composer::Value>{Composer::Error(e.what())}});
            }
        });
        return Composer::Value(composerPromise);
    }
    static const Composer::ValueSchema& schema() {
        static auto schema = Composer::ValueSchema::promise(schemaOrRef<RESULT>());
        return schema;
    }
};

template<typename U>
struct ExceptionHandlingTraits<FutureAdaptor<U>> {
    static Composer::Value handleNativeException(const std::exception& e, const Composer::ValueFunctionCallContext& callContext) {
        // store C++ exception in JS Error and raise in JS runtime
        auto msg = STRING_FORMAT("C++: {}", e.what());
        auto composerPromise = Composer::makeShared<Composer::ResolvablePromise>();
        composerPromise->fulfill(Composer::Result<Composer::Value>(Composer::Error(std::move(msg))));
        return Composer::Value(composerPromise);
    }
    static Composer::Value handleNativeException(const JsException& e, const Composer::ValueFunctionCallContext& callContext) {
        // JS error passthrough
        auto composerPromise = Composer::makeShared<Composer::ResolvablePromise>();
        composerPromise->fulfill(Composer::Result<Composer::Value>(e.cause()));
        return Composer::Value(composerPromise);
    }
};

} // namespace djinni

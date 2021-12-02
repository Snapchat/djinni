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

#include "DJIMarshal+Private.h"
#import "DJFuture.h"
#import "Future.hpp"

namespace snapchat {
namespace djinni {


template <class RESULT>
class FutureAdaptor
{
    using CppResType = typename RESULT::CppType;

public:
    using CppType = Future<CppResType>;
    using ObjcType = DJFuture*;

    using Boxed = FutureAdaptor;

    static CppType toCpp(ObjcType o)
    {
        using NativePromiseType = Promise<CppResType>;

        __block auto p = std::make_unique<NativePromiseType>();
        auto f = p->getFuture();
        [o then: ^id(DJFuture* res) {
                @try {
                    p->setValue(RESULT::Boxed::toCpp([res get]));
                } @catch (NSException* e) {
                    p->setException(std::runtime_error(::djinni::String::toCpp(e.reason)));
                }
                return nil;
            }];
        return f;
    }

    static ObjcType fromCpp(CppType c)
    {
        DJPromise<typename RESULT::Boxed::ObjcType>* promise = [[DJPromise alloc] init];
        DJFuture<typename RESULT::Boxed::ObjcType>* future = [promise getFuture];

        c.then([promise] (Future<CppResType> res) {
                try {
                    [promise setValue:RESULT::Boxed::fromCpp(res.get())];
                } catch (std::exception& e) {
                    [promise setException: [NSException exceptionWithName:@"" reason: ::djinni::String::fromCpp(e.what()) userInfo:nil]];
                }
            });
        
        return future;
    }
};

}} // namespace snapchat::djinni

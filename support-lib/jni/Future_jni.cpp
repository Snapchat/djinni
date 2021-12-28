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

#include "Future_jni.hpp"

namespace djinni {

// NOLINTNEXTLINE
static void NativeFutureHandler_nativeHandleResult(JNIEnv* jniEnv, jclass /*unused*/, jlong nativeFunc, jlong nativePromise, jobject jres, jthrowable jex) {
    auto func = reinterpret_cast<NativeFutureHandlerFunc>(nativeFunc);
    func(jniEnv, nativePromise, jres, jex);
}

static const JNINativeMethod kNativeMethods[] = {{
    const_cast<char*>("nativeHandleResult"),
    const_cast<char*>("(JJLjava/lang/Object;Ljava/lang/Throwable;)V"),
    reinterpret_cast<void*>(&NativeFutureHandler_nativeHandleResult),
}};

// NOLINTNEXTLINE
static auto sRegisterMethods =
    JNIMethodLoadAutoRegister("com/snapchat/djinni/NativeFutureHandler", kNativeMethods);

} // namespace djinni

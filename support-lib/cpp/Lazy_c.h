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

#include "djinni_c.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef djinni_ref djinni_lazy_ref;

// Call the lazy function to get its value
djinni_ref djinni_lazy_call(djinni_lazy_ref lazy);

// Create a lazy from a C function pointer
typedef djinni_ref (*djinni_lazy_func_ptr)(void* context);
typedef void (*djinni_lazy_context_releaser)(void*);
djinni_lazy_ref djinni_lazy_make(djinni_lazy_func_ptr func, void* context, djinni_lazy_context_releaser context_releaser);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

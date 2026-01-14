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

#include "Lazy_c.h"
#include "djinni_c_types.hpp"

namespace djinni {

class Lazy : public Object {
public:
  Lazy(djinni_lazy_func_ptr func, void* context, djinni_lazy_context_releaser context_releaser)
      : _func(func), _context(context), _context_releaser(context_releaser) {}

  ~Lazy() override {
    if (_context_releaser && _context) {
      _context_releaser(_context);
    }
  }

  djinni_ref call() const {
    return _func(_context);
  }

private:
  djinni_lazy_func_ptr _func;
  void* _context;
  djinni_lazy_context_releaser _context_releaser;
};

} // namespace djinni

using namespace djinni;

djinni_ref djinni_lazy_call(djinni_lazy_ref lazy) {
  return fromC<Lazy>(lazy)->call();
}

djinni_lazy_ref djinni_lazy_make(djinni_lazy_func_ptr func, void* context, djinni_lazy_context_releaser context_releaser) {
  return toC(new Lazy(func, context, context_releaser));
}

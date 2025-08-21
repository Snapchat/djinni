#pragma once

#include "Future.hpp"
#include "djinni_c_types.hpp"
#include "future_c.h"

namespace djinni {
template <typename T> class FutureHolder : public Object {
public:
  FutureHolder(Future<T> future) : _future(std::move(future)) {}
  ~FutureHolder() override = default;

  Future<T> &getFuture() { return _future; }

private:
  Future<T> _future;
};
} // namespace djinni

namespace djinni::c_api {

template <typename Tr> struct FutureTranslator {
  using CppType = ::djinni::Future<typename Tr::CppType>;
  using CType = djinni_future_ref;

  static CppType toCpp(CType future) {
    auto *futureHolder =
        fromC<::djinni::FutureHolder<typename Tr::CppType>>(future);

    // Any way to make this better?

    return futureHolder->getFuture().then(
        [](CppType value) { return value.get(); });
  }

  static CType fromCpp(CppType &&future) {
    auto *futureHolder = new ::djinni::FutureHolder<typename Tr::CppType>(std::move(future));

    return toC(futureHolder);
  }
};

} // namespace djinni::c_api
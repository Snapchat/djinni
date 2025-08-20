#pragma once

#include "Future.hpp"
#include "djinni_c.h"
#include "djinni_c_types.hpp"

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

template <typename T> class FutureTranslator {
public:
  static ::djinni::Future<T> toCpp(djinni_future_ref future) {
    auto *futureHolder = static_cast<::djinni::FutureHolder<T> *>(
        reinterpret_cast<Object *>(future));

    // Any way to make this better?

    return futureHolder->getFuture().then(
        [](::djinni::Future<T> value) { return value.get(); });
  }

  static djinni_future_ref fromCpp(::djinni::Future<T> &&future) {
    Object *futureHolder = new ::djinni::FutureHolder<T>(std::move(future));

    return reinterpret_cast<djinni_future_ref>(futureHolder);
  }
};

} // namespace djinni::c_api
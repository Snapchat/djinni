#pragma once

#include "djinni_c.h"
#include <memory>
#include <optional>

namespace djinni::c_api {

template <typename T> class Record {
public:
  template <typename... Args> static djinni_record_ptr make(Args &&...args) {
    return reinterpret_cast<djinni_record_ptr>(
        new T(std::forward<Args>(args)...));
  }

  static T *toCpp(djinni_record_ptr ptr) { return reinterpret_cast<T *>(ptr); }

  static djinni_record_ptr fromCpp(T &&value) { return make(std::move(value)); }

  static void release(djinni_record_ptr ptr) { delete toCpp(ptr); }
};

class Optional {
public:
  static djinni_optional_bool fromCpp(std::optional<bool> value) {
    return fromCppPrimitive<djinni_optional_bool>(value);
  }

  static djinni_optional_int8_t fromCpp(std::optional<int8_t> value) {
    return fromCppPrimitive<djinni_optional_int8_t>(value);
  }

  static djinni_optional_int16_t fromCpp(std::optional<int16_t> value) {
    return fromCppPrimitive<djinni_optional_int16_t>(value);
  }

  static djinni_optional_int32_t fromCpp(std::optional<int32_t> value) {
    return fromCppPrimitive<djinni_optional_int32_t>(value);
  }

  static djinni_optional_int64_t fromCpp(std::optional<int64_t> value) {
    return fromCppPrimitive<djinni_optional_int64_t>(value);
  }

  static djinni_optional_float fromCpp(std::optional<float> value) {
    return fromCppPrimitive<djinni_optional_float>(value);
  }

  static djinni_optional_double fromCpp(std::optional<double> value) {
    return fromCppPrimitive<djinni_optional_double>(value);
  }

private:
  template <typename Opt, typename T>
  static Opt fromCppPrimitive(std::optional<T> value) {
    Opt out = {0};

    if (value.has_value()) {
      out.has_value = true;
      out.value = value.value();
    }

    return out;
  }
};



} // namespace djinni::c_api
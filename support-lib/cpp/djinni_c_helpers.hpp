#pragma once

#include "djinni_c.h"
#include <atomic>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace djinni::c_api {

class Object {
public:
  Object();
  virtual ~Object();

private:
  std::atomic_int _ref;
};

class String {
public:
  static djinni_string_ref fromCpp(std::string &&str);
  static djinni_string_ref fromCpp(const std::string &str);
  static std::string toCpp(djinni_string_ref str);
};

class Date {
public:
  static djinni_date_ref
  fromCpp(const std::chrono::system_clock::time_point &date);
  static std::chrono::system_clock::time_point toCpp(djinni_date_ref date);
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

  template <typename T, typename F>
  static djinni_ref fromCpp(const std::optional<T> &value, F &&convert) {
    if (!value) {
      return nullptr;
    } else {
      return convert(value.value());
    }
  }

  template <typename T, typename F>
  static djinni_ref fromCpp(std::optional<T> &&value, F &&convert) {
    if (!value) {
      return nullptr;
    } else {
      return convert(std::move(value.value()));
    }
  }

  template <typename F>
  static auto toCpp(djinni_ref ptr, F &&convert)
      -> std::optional<std::invoke_result_t<F, djinni_ref>> {
    if (ptr == nullptr) {
      return std::nullopt;
    } else {
      return std::optional<T>(convert(ptr));
    }
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

template <typename Cpp, typename C> class Enum {
public:
  static Cpp toCpp(C value) { return static_cast<Cpp>(value); }
  static C fromCpp(Cpp value) { return static_cast<C>(value); }

  static djinni_number_ref toCppBoxed(C value) {
    return Number::toCpp(static_cast<int64_t>(value));
  }

  static C fromCpp(djinni_number_ref value) {
    return static_cast<C>(djinni_number_get_int64(value));
  }
};

template <typename T> class List {
public:
  template <typename F>
  static std::vector<T> toCpp(djinni_array_ref value, F &&convert) {
    std::vector<T> output;
    auto length = djinni_array_get_length(value);
    output.reserve(length);

    for (size_t i = 0; i < length; i++) {
      auto value = djinni_array_get_value(value, i);
      output.emplace_back(convert(value));
      djinni_ref_release(value);
    }

    return output;
  }

  template <typename F>
  static djinni_array_ref fromCpp(const std::vector<T> &values, F &&convert) {
    djinni_array_ref output = djinni_array_create(values.size());
    size_t index = 0;

    for (const auto &value : values) {
      auto converted = convert(value);
      djinni_array_set_value(output, index++, converted);
      djinni_ref_release(converted);
    }

    return output;
  }
};

template <typename T> class Set {
public:
  template <typename F>
  static std::unordered_set<T> toCpp(djinni_array_ref value, F &&convert) {
    std::unordered_set<T> output;
    auto length = djinni_array_get_length(value);
    output.reserve(length);

    for (size_t i = 0; i < length; i++) {
      auto value = djinni_array_get_value(value, i);
      output.emplace(convert(value));
      djinni_ref_release(value);
    }

    return output;
  }

  template <typename F>
  static djinni_array_ref fromCpp(const std::unordered_set<T> &values,
                                  F &&convert) {
    djinni_array_ref output = djinni_array_create(values.size());
    size_t index = 0;

    for (const auto &value : values) {
      auto converted = convert(value);
      djinni_array_set_value(output, index++, converted);
      djinni_ref_release(converted);
    }

    return output;
  }
};

template <typename K, typename V> class Map {
public:
  template <typename F>
  static std::unordered_map<K, V> toCpp(djinni_keyval_array_ref key_values,
                                        F &&convert) {
    std::unordered_map<K, V> output;
    auto length = djinni_keyval_array_get_length(key_values);
    output.reserve(length);

    for (size_t i = 0; i < length; i++) {
      auto key = djinni_keyval_array_get_key(key_values, i);
      auto value = djinni_keyval_array_get_value(key_values, i);
      auto pair = convert(key, value);
      output.try_emplace(std::move(pair.first), std::move(pair.second));
      djinni_ref_release(key);
      djinni_ref_release(value);
    }

    return output;
  }

  template <typename F>
  static djinni_keyval_array_ref fromCpp(const std::unordered_map<K, V> &map,
                                         F &&convert) {
    djinni_keyval_array_ref output = djinni_keyval_array_create(map.size());
    size_t index = 0;
    for (const auto &it : map) {
      auto pair = convert(it.first, it.value);

      djinni_keyval_array_set_entry(output, index++, pair.first, pair.second);

      djinni_ref_release(pair.first);
      djinni_ref_release(pair.second);
    }
    return output;
  }
};

} // namespace djinni::c_api
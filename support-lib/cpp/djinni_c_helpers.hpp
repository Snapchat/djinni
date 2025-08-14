#pragma once

#include "djinni_c.h"
#include "djinni_c_types.hpp"
#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace djinni::c_api {

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

class Number {
public:
  template <typename T> static djinni_number_ref fromCpp(T value) = delete;
  template <typename T> static T toCpp(djinni_number_ref value) = delete;

  template <> djinni_number_ref fromCpp(uint8_t value) {
    return fromCppUnsignedInt(value);
  }
  template <> djinni_number_ref fromCpp(uint16_t value) {
    return fromCppUnsignedInt(value);
  }
  template <> djinni_number_ref fromCpp(uint32_t value) {
    return fromCppUnsignedInt(value);
  }
  template <> djinni_number_ref fromCpp(uint64_t value) {
    return fromCppUnsignedInt(value);
  }
  template <> djinni_number_ref fromCpp(bool value) {
    return fromCppUnsignedInt(value);
  }

  template <> uint8_t toCpp(djinni_number_ref value) {
    return toCppUnsignedInt<uint8_t>(value);
  }
  template <> uint16_t toCpp(djinni_number_ref value) {
    return toCppUnsignedInt<uint16_t>(value);
  }
  template <> uint32_t toCpp(djinni_number_ref value) {
    return toCppUnsignedInt<uint32_t>(value);
  }
  template <> uint64_t toCpp(djinni_number_ref value) {
    return toCppUnsignedInt<uint64_t>(value);
  }
  template <> bool toCpp(djinni_number_ref value) {
    return toCppUnsignedInt<bool>(value);
  }

  template <> djinni_number_ref fromCpp(int8_t value) {
    return fromCppSignedInt(value);
  }
  template <> djinni_number_ref fromCpp(int16_t value) {
    return fromCppSignedInt(value);
  }
  template <> djinni_number_ref fromCpp(int32_t value) {
    return fromCppSignedInt(value);
  }
  template <> djinni_number_ref fromCpp(int64_t value) {
    return fromCppSignedInt(value);
  }

  template <> int8_t toCpp(djinni_number_ref value) {
    return toCppSignedInt<int8_t>(value);
  }
  template <> int16_t toCpp(djinni_number_ref value) {
    return toCppSignedInt<int16_t>(value);
  }
  template <> int32_t toCpp(djinni_number_ref value) {
    return toCppSignedInt<int32_t>(value);
  }
  template <> int64_t toCpp(djinni_number_ref value) {
    return toCppSignedInt<int64_t>(value);
  }

  template <> djinni_number_ref fromCpp(float value) {
    return djinni_number_double_create(value);
  }

  template <> djinni_number_ref fromCpp(double value) {
    return djinni_number_double_create(value);
  }

  template <> float toCpp(djinni_number_ref value) {
    return static_cast<float>(djinni_number_get_double(value));
  }

  template <> double toCpp(djinni_number_ref value) {
    return djinni_number_get_double(value);
  }

private:
  template <typename T> static djinni_number_ref fromCppUnsignedInt(T value) {
    return djinni_number_uint64_create(static_cast<uint64_t>(value));
  }

  template <typename T> static T toCppUnsignedInt(djinni_number_ref value) {
    return static_cast<T>(djinni_number_get_uint64(value));
  }

  template <typename T> static djinni_number_ref fromCppSignedInt(T value) {
    return djinni_number_int64_create(static_cast<int64_t>(value));
  }

  template <typename T> static T toCppSignedInt(djinni_number_ref value) {
    return static_cast<T>(djinni_number_get_int64(value));
  }
};

class Optional {
public:
  template <typename Opt, typename T> static T fromCppPrimitive(Opt value) {
    T out = {0};

    if (value) {
      out.has_value = true;
      out.value = value.value();
    }

    return out;
  }

  template <typename Opt, typename T> static Opt toCppPrimitive(T value) {
    if (value.has_value) {
      return Opt(value.value);
    } else {
      return Opt();
    }
  }

  template <typename Opt, typename F>
  static djinni_ref fromCpp(const Opt &value, F &&convert) {
    if (!value) {
      return nullptr;
    } else {
      return convert(value.value());
    }
  }

  template <typename Opt, typename F>
  static djinni_ref fromCpp(Opt &&value, F &&convert) {
    if (!value) {
      return nullptr;
    } else {
      return convert(std::move(value.value()));
    }
  }

  template <typename Opt, typename F>
  static Opt toCpp(djinni_ref ptr, F &&convert) {
    if (ptr == nullptr) {
      return Opt();
    } else {
      return Opt(convert(ptr));
    }
  }

private:
};

template <typename T> class Record {
public:
  template <typename... Args> static djinni_record_ref make(Args &&...args) {
    Object *obj = new RecordHolder<T>(T(std::forward<Args>(args)...));
    return reinterpret_cast<djinni_record_ref>(obj);
  }

  static T *toCpp(djinni_record_ref ptr) {
    auto *record =
        static_cast<RecordHolder<T> *>(reinterpret_cast<Object *>(ptr));
    if (record == nullptr) {
      return nullptr;
    }

    return &record->data();
  }

  static djinni_record_ref fromCpp(T &&value) { return make(std::move(value)); }

  static djinni_record_ref fromCpp(const T &value) { return make(value); }

  static void release(djinni_record_ref ptr) {
    Object::release(reinterpret_cast<Object *>(ptr));
  }
};

template <typename Cpp, typename C> class Enum {
public:
  static Cpp toCpp(C value) { return static_cast<Cpp>(value); }
  static C fromCpp(Cpp value) { return static_cast<C>(value); }

  template <typename Opt> static Opt toCppBoxed(djinni_number_ref value) {
    return value != nullptr ? Opt(static_cast<Cpp>(djinni_number_get_int64(value)))
                            : Opt();
  }

  template <typename Opt> static djinni_number_ref fromCppBoxed(Opt value) {
    return value ? Number::fromCpp(static_cast<int64_t>(value.value()))
                 : nullptr;
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
      auto item = djinni_array_get_value(value, i);
      output.emplace_back(convert(item));
      djinni_ref_release(item);
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
      auto item = djinni_array_get_value(value, i);
      output.emplace(convert(item));
      djinni_ref_release(item);
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
      auto pair = convert(it.first, it.second);

      djinni_keyval_array_set_entry(output, index++, pair.first, pair.second);

      djinni_ref_release(pair.first);
      djinni_ref_release(pair.second);
    }
    return output;
  }
};

} // namespace djinni::c_api
#pragma once

#include "DataRef.hpp"
#include "DataView.hpp"
#include "Future.hpp"
#include "djinni_c.h"
#include "djinni_c_types.hpp"
#include "expected.hpp"
#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
    return djinni_number_double_new(value);
  }

  template <> djinni_number_ref fromCpp(double value) {
    return djinni_number_double_new(value);
  }

  template <> float toCpp(djinni_number_ref value) {
    return static_cast<float>(djinni_number_get_double(value));
  }

  template <> double toCpp(djinni_number_ref value) {
    return djinni_number_get_double(value);
  }

private:
  template <typename T> static djinni_number_ref fromCppUnsignedInt(T value) {
    return djinni_number_uint64_new(static_cast<uint64_t>(value));
  }

  template <typename T> static T toCppUnsignedInt(djinni_number_ref value) {
    return static_cast<T>(djinni_number_get_uint64(value));
  }

  template <typename T> static djinni_number_ref fromCppSignedInt(T value) {
    return djinni_number_int64_new(static_cast<int64_t>(value));
  }

  template <typename T> static T toCppSignedInt(djinni_number_ref value) {
    return static_cast<T>(djinni_number_get_int64(value));
  }
};

class Optional {
public:
  template <typename Opt, typename T> static T fromCppPrimitive(Opt value) {
    T out;

    if (value) {
      out.has_value = true;
      out.value = value.value();
    } else {
      out.has_value = false;
      out.value = 0;
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

  template <typename T, typename F>
  static djinni_ref fromSharedPtrCpp(const T &value, F &&convert) {
    if (value == nullptr) {
      return nullptr;
    } else {
      return convert(value);
    }
  }

  template <typename T, typename F>
  static djinni_ref fromSharedPtrCpp(T &&value, F &&convert) {
    if (value == nullptr) {
      return nullptr;
    } else {
      return convert(std::move(value));
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

  template <typename T, typename F>
  static T toSharedPtrCpp(djinni_ref ptr, F &&convert) {
    if (ptr == nullptr) {
      return T();
    } else {
      return convert(ptr);
    }
  }

private:
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
    djinni_array_ref output = djinni_array_new(values.size());
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
    djinni_array_ref output = djinni_array_new(values.size());
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
    djinni_keyval_array_ref output = djinni_keyval_array_new(map.size());
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

template <typename T> class Record {
public:
  template <typename... Args> static djinni_record_ref make(Args &&...args) {
    Object *obj = new RecordHolder<T>(T(std::forward<Args>(args)...));
    return reinterpret_cast<djinni_record_ref>(obj);
  }

  static T &toCpp(djinni_record_ref ref) {
    auto *record =
        static_cast<RecordHolder<T> *>(reinterpret_cast<Object *>(ref));
    if (record == nullptr) {
      std::abort();
    }

    return record->data();
  }

  static djinni_record_ref fromCpp(T &&value) { return make(std::move(value)); }

  static djinni_record_ref fromCpp(const T &value) { return make(value); }

  static void release(djinni_record_ref ptr) {
    Object::release(reinterpret_cast<Object *>(ptr));
  }
};

template <typename T> class Interface {
public:
  static const std::shared_ptr<T> &toCpp(djinni_interface_ref ref) {
    auto *i =
        static_cast<InterfaceHolder<T> *>(reinterpret_cast<Object *>(ref));
    if (i == nullptr) {
      std::abort();
    }

    return i->data();
  }

  static djinni_interface_ref fromCpp(std::shared_ptr<T> value) {
    Object *obj = new InterfaceHolder<T>(std::move(value));
    return reinterpret_cast<djinni_interface_ref>(obj);
  }
};

template <typename T> class ProxyClass {
public:
  static djinni_proxy_class_ref
  make(const T *methodDefs, djinni_opaque_deallocator opaqueDeallocator) {
    Object *obj = new ::djinni::ProxyClass<T>(*methodDefs, opaqueDeallocator);
    return reinterpret_cast<djinni_proxy_class_ref>(obj);
  }
};

template <typename T, typename PT> class Proxy {
public:
  static djinni_interface_ref make(djinni_proxy_class_ref proxyClassRef,
                                   void *opaque) {
    auto *proxyClass = static_cast<::djinni::ProxyClass<PT> *>(
        reinterpret_cast<Object *>(proxyClassRef));

    return ::djinni::c_api::Interface<T>::fromCpp(
        std::make_shared<T>(proxyClass, opaque));
  }
};

template <typename Cpp, typename C> class Enum {
public:
  static Cpp toCpp(C value) { return static_cast<Cpp>(value); }
  static C fromCpp(Cpp value) { return static_cast<C>(value); }

  static Cpp toCppBoxed(djinni_number_ref value) {
    return static_cast<Cpp>(djinni_number_get_int64(value));
  }

  static djinni_number_ref fromCppBoxed(Cpp value) {
    return Number::fromCpp(static_cast<int64_t>(value));
  }
};

class DataRef {
public:
  static ::djinni::DataRef toCpp(djinni_binary_ref binary);
  static djinni_binary_ref fromCpp(const ::djinni::DataRef &dataRef);
};

class DataView {
public:
  static ::djinni::DataView toCpp(djinni_binary_ref binary);
  static djinni_binary_ref fromCpp(const ::djinni::DataView &dataRef);
};

class Binary {
public:
  static std::vector<uint8_t> toCpp(djinni_binary_ref binary);
  static djinni_binary_ref fromCpp(std::vector<uint8_t> &&binary);
  static djinni_binary_ref fromCpp(const std::vector<uint8_t> &binary);
};

template <typename T> class Future {
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

template <typename T, typename E> class Outcome {
public:
  static ::djinni::expected<T, E> toCpp(djinni_outcome_ref future) {
    std::abort();
  }

  static djinni_outcome_ref fromCpp(::djinni::expected<T, E> &&outcome) {
    std::abort();
  }

  static djinni_outcome_ref fromCpp(const ::djinni::expected<T, E> &outcome) {
    std::abort();
  }
};

template <class Rep, class Ratio> class Duration {
public:
  static std::chrono::duration<Rep, Ratio> toCpp(djinni_number_ref value);
  static djinni_number_ref
  fromCpp(const std::chrono::duration<Rep, Ratio> &value);
};

template <typename T> class Protobuf {
public:
  static T toCpp(djinni_binary_ref binary) {
    T output;
    output.ParseFromArray(djinni_binary_get_data(binary),
                          djinni_binary_get_length(binary));
    return output;
  }

  static djinni_binary_ref fromCpp(const T &proto) {
    auto length = proto.ByteSizeLong();
    auto output = djinni_binary_new(static_cast<size_t>(length));

    proto.SerializeToArray(djinni_binary_get_data(output), length);

    return output;
  }
};

} // namespace djinni::c_api
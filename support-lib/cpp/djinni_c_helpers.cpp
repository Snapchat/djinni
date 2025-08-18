#include "djinni_c_helpers.hpp"

namespace djinni::c_api {

class BinaryWithDataRef : public ::djinni::Binary {
public:
  BinaryWithDataRef(const ::djinni::DataRef &dataRef)
      : ::djinni::Binary(dataRef.mutableBuf(), dataRef.len()),
        _dataRef(dataRef) {}

  ~BinaryWithDataRef() override = default;

private:
  djinni::DataRef _dataRef;
};

class BinaryWithVector : public ::djinni::Binary {
public:
  BinaryWithVector(std::vector<uint8_t> &&vec)
      : ::djinni::Binary(vec.data(), vec.size()), _vec(std::move(vec)) {}

  ~BinaryWithVector() override = default;

private:
  std::vector<uint8_t> _vec;
};

class DataRefImpl : public ::djinni::DataRef::Impl {
public:
  DataRefImpl(djinni_binary_ref binary) : _binary(binary) {
    djinni_ref_retain(binary);
  }

  ~DataRefImpl() override {
    djinni_ref_release(_binary);
  }

  const uint8_t *buf() const override {
    return djinni_binary_get_data(_binary);
  }

  size_t len() const override { return djinni_binary_get_length(_binary); }

  uint8_t *mutableBuf() override { return djinni_binary_get_data(_binary); }

private:
  djinni_binary_ref _binary;
};

djinni_string_ref String::fromCpp(std::string &&str) {
  return djinni_string_new(str.c_str(), str.length());
}

djinni_string_ref String::fromCpp(const std::string &str) {
  return djinni_string_new(str.c_str(), str.length());
}

std::string String::toCpp(djinni_string_ref str) {
  const auto *data = djinni_string_get_data(str);
  auto length = djinni_string_get_length(str);

  return std::string(data, length);
}

::djinni::DataRef DataRef::toCpp(djinni_binary_ref binary) {
  return ::djinni::DataRef(std::make_shared<DataRefImpl>(binary));
}

djinni_binary_ref DataRef::fromCpp(const ::djinni::DataRef &dataRef) {
  Object *obj = new BinaryWithDataRef(dataRef);
  return reinterpret_cast<djinni_binary_ref>(obj);
}

::djinni::DataView DataView::toCpp(djinni_binary_ref binary) {
  return djinni::DataView(djinni_binary_get_data(binary),
                          djinni_binary_get_length(binary));
}

djinni_binary_ref DataView::fromCpp(const ::djinni::DataView &dataRef) {
  return djinni_binary_new_with_bytes(dataRef.buf(), dataRef.len(), nullptr, nullptr);
}

std::vector<uint8_t> Binary::toCpp(djinni_binary_ref binary) {
  const auto *data = djinni_binary_get_data(binary);
  return std::vector<uint8_t>(data, data + djinni_binary_get_length(binary));
}

djinni_binary_ref Binary::fromCpp(std::vector<uint8_t> &&binary) {
  Object *obj = new BinaryWithVector(std::move(binary));
  return reinterpret_cast<djinni_binary_ref>(obj);
}

djinni_binary_ref Binary::fromCpp(const std::vector<uint8_t> &binary) {
  return djinni_binary_new_with_bytes_copy(binary.data(), binary.size());
}

} // namespace djinni::c_api
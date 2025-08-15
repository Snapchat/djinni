#include "djinni_c_helpers.hpp"

namespace djinni::c_api {

djinni_string_ref String::fromCpp(std::string &&str) {
  return djinni_string_create(str.c_str(), str.length());
}

djinni_string_ref String::fromCpp(const std::string &str) {
  return djinni_string_create(str.c_str(), str.length());
}

std::string String::toCpp(djinni_string_ref str) {
  const auto *data = djinni_string_get_data(str);
  auto length = djinni_string_get_length(str);

  return std::string(data, length);
}

} // namespace djinni::c_api
#pragma once

namespace djinni::c_api {

template <class Rep, class Ratio> class DurationTranslator {
public:
  static std::chrono::duration<Rep, Ratio> toCpp(djinni_number_ref value);
  static djinni_number_ref
  fromCpp(const std::chrono::duration<Rep, Ratio> &value);
};

} // namespace djinni::c_api

#pragma once

#include "Outcome_c.h"
#include "expected.hpp"

namespace djinni::c_api {

template <typename T, typename E> class OutcomeTranslator {
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

} // namespace djinni::c_api
#pragma once

#include "Outcome_c.h"
#include "expected.hpp"

namespace djinni::c_api {

template <typename T, typename E> class OutcomeTranslator {
public:
  template <typename F1, typename F2>
  static ::djinni::expected<T, E> toCpp(djinni_outcome_ref outcome, F1 &&toCpp1,
                                        F2 &&toCpp2) {
    if (djinni_outcome_is_success(outcome)) {
      auto success = djinni_outcome_get_value(outcome);
      auto out = ::djinni::expected<T, E>(toCpp1(success));
      djinni_ref_release(success);
      return out;
    } else {
      auto error = djinni_outcome_get_error(outcome);
      auto out = ::djinni::make_unexpected<E>(toCpp2(error));
      djinni_ref_release(error);
      return out;
    }
  }

  template <typename F1, typename F2>
  static djinni_outcome_ref fromCpp(::djinni::expected<T, E> &&outcome,
                                    F1 &&fromCpp1, F2 &&fromCpp2) {
    if (outcome) {
      auto success = fromCpp1(std::move(outcome.value()));
      djinni_outcome_ref result = djinni_outcome_make_success(success);
      djinni_ref_release(success);
      return result;
    } else {
      auto error = fromCpp2(std::move(outcome.error()));
      djinni_outcome_ref result = djinni_outcome_make_error(error);
      djinni_ref_release(error);
      return result;
    }
  }

  template <typename F1, typename F2>
  static djinni_outcome_ref fromCpp(const ::djinni::expected<T, E> &outcome,
                                    F1 &&fromCpp1, F2 &&fromCpp2) {
    if (outcome) {
      auto success = fromCpp1(outcome.value());
      djinni_outcome_ref result = djinni_outcome_make_success(success);
      djinni_ref_release(success);
      return result;
    } else {
      auto error = fromCpp2(outcome.error());
      djinni_outcome_ref result = djinni_outcome_make_error(error);
      djinni_ref_release(error);
      return result;
    }
  }
};

} // namespace djinni::c_api
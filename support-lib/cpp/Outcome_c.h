#pragma once

#include "djinni_c.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef djinni_ref djinni_outcome_ref;

djinni_outcome_ref djinni_outcome_make_success(djinni_ref success_val);
djinni_outcome_ref djinni_outcome_make_error(djinni_ref error_val);

bool djinni_outcome_is_success(djinni_outcome_ref outcome);
bool djinni_outcome_is_error(djinni_outcome_ref outcome);

djinni_ref djinni_outcome_get_value(djinni_outcome_ref outcome);
djinni_ref djinni_outcome_get_error(djinni_outcome_ref outcome);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
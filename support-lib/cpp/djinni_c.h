#pragma once

#include <stdint.h>

typedef void *djinni_record_ptr;

#define DJINNI_OPTIONAL_PRIMITIVE(__type__) typedef struct { \
  __type__ value; \
  bool has_value; \
} djinni_optional_ ## __type__

DJINNI_OPTIONAL_PRIMITIVE(bool);
DJINNI_OPTIONAL_PRIMITIVE(int8_t);
DJINNI_OPTIONAL_PRIMITIVE(int16_t);
DJINNI_OPTIONAL_PRIMITIVE(int32_t);
DJINNI_OPTIONAL_PRIMITIVE(int64_t);
DJINNI_OPTIONAL_PRIMITIVE(float);
DJINNI_OPTIONAL_PRIMITIVE(double);

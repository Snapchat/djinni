#pragma once

#include <stdint.h>
#include <stddef.h>

typedef void *djinni_ref;

typedef djinni_ref djinni_string_ref;
typedef djinni_ref djinni_binary_ref;
typedef djinni_ref djinni_number_ref;
typedef djinni_ref djinni_array_ref;
typedef djinni_ref djinni_keyval_array_ref;
typedef djinni_ref djinni_date_ref;

typedef djinni_ref djinni_record_ref;
typedef djinni_ref djinni_interface_ref;

typedef void(*djinni_binary_deallocator)(uint8_t *, size_t, void *);

void djinni_ref_retain(djinni_ref ref);
void djinni_ref_release(djinni_ref ref);

djinni_string_ref djinni_string_create(const char *str, size_t length);

const char *djinni_string_get_data(djinni_string_ref str);

size_t djinni_string_get_length(djinni_string_ref str);

djinni_binary_ref djinni_binary_create(uint8_t *data, size_t length, void *opaque, djinni_binary_deallocator deallocator);

djinni_number_ref djinni_number_int64_create(int64_t v);
djinni_number_ref djinni_number_uint64_create(uint64_t v);
djinni_number_ref djinni_number_double_create(double v);

int64_t djinni_number_get_int64(djinni_number_ref number);
uint64_t djinni_number_get_uint64(djinni_number_ref number);
double djinni_number_get_double(djinni_number_ref number);

djinni_keyval_array_ref djinni_keyval_array_create(size_t size);

size_t djinni_keyval_array_get_length(djinni_keyval_array_ref keyval_array);
djinni_ref djinni_keyval_array_get_key(djinni_keyval_array_ref keyval_array, size_t index);
djinni_ref djinni_keyval_array_get_value(djinni_keyval_array_ref keyval_array, size_t index);

void djinni_keyval_array_set_entry(djinni_keyval_array_ref keyval_array, size_t index, djinni_ref key, djinni_ref value);

djinni_array_ref djinni_array_create(size_t length);
size_t djinni_array_get_length(djinni_array_ref array);
djinni_ref djinni_array_get_value(djinni_array_ref array, size_t index);
void djinni_array_set_value(djinni_array_ref array, size_t index, djinni_ref value);

djinni_date_ref djinni_date_create(uint64_t epoch_time_ms);
uint64_t djinni_date_get_epoch(djinni_date_ref date);

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
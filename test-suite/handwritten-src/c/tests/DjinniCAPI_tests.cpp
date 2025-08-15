#include "assorted_primitives.h"
#include "client_returned_record.h"
#include "map_record.h"
#include "primitive_list.h"
#include "gtest/gtest.h"

namespace djinni {

template <typename T> struct CRef {
  T value;
  CRef(T value) : value(value) {}

  ~CRef() { djinni_ref_release(value); }
};

TEST(DjinniCAPI, supportsPrimitiveValues) {
  auto primitives = CRef(testsuite_assorted_primitives_create(
      true, 8, 4242, 3000000, 99999999999, 32.5f, 6482000.5,
      djinni_optional_bool_empty(), djinni_optional_int8_t_empty(),
      djinni_optional_int16_t_empty(), djinni_optional_int32_t_empty(),
      djinni_optional_int64_t_empty(), djinni_optional_float_empty(),
      djinni_optional_double_empty()));

  ASSERT_TRUE(testsuite_assorted_primitives_get_b(primitives.value));
  testsuite_assorted_primitives_set_b(primitives.value, false);
  ASSERT_FALSE(testsuite_assorted_primitives_get_b(primitives.value));

  ASSERT_EQ(8, testsuite_assorted_primitives_get_eight(primitives.value));
  testsuite_assorted_primitives_set_eight(primitives.value, 16);
  ASSERT_EQ(16, testsuite_assorted_primitives_get_eight(primitives.value));

  ASSERT_EQ(4242, testsuite_assorted_primitives_get_sixteen(primitives.value));
  testsuite_assorted_primitives_set_sixteen(primitives.value, 4343);
  ASSERT_EQ(4343, testsuite_assorted_primitives_get_sixteen(primitives.value));

  ASSERT_EQ(3000000,
            testsuite_assorted_primitives_get_thirtytwo(primitives.value));
  testsuite_assorted_primitives_set_thirtytwo(primitives.value, 6000000);
  ASSERT_EQ(6000000,
            testsuite_assorted_primitives_get_thirtytwo(primitives.value));

  ASSERT_EQ(99999999999,
            testsuite_assorted_primitives_get_sixtyfour(primitives.value));
  testsuite_assorted_primitives_set_sixtyfour(primitives.value, 999999999990);
  ASSERT_EQ(999999999990,
            testsuite_assorted_primitives_get_sixtyfour(primitives.value));

  ASSERT_EQ(32.5f,
            testsuite_assorted_primitives_get_fthirtytwo(primitives.value));
  testsuite_assorted_primitives_set_fthirtytwo(primitives.value, 64.5f);
  ASSERT_EQ(64.5f,
            testsuite_assorted_primitives_get_fthirtytwo(primitives.value));

  ASSERT_EQ(6482000.5,
            testsuite_assorted_primitives_get_fsixtyfour(primitives.value));
  testsuite_assorted_primitives_set_fsixtyfour(primitives.value, 0.00001);
  ASSERT_EQ(0.00001,
            testsuite_assorted_primitives_get_fsixtyfour(primitives.value));
}

TEST(DjinniCAPI, supportsOptionalPrimitiveValues) {
  auto primitives = CRef(testsuite_assorted_primitives_create(
      false, 0, 0, 0, 0, 0.0f, 0.0, djinni_optional_bool_make(true),
      djinni_optional_int8_t_make(8), djinni_optional_int16_t_make(16),
      djinni_optional_int32_t_make(4242), djinni_optional_int64_t_make(3000000),
      djinni_optional_float_make(32.5f),
      djinni_optional_double_make(6482000.5)));

  // Test optional bool
  ASSERT_TRUE(
      testsuite_assorted_primitives_get_o_b(primitives.value).has_value);
  ASSERT_TRUE(testsuite_assorted_primitives_get_o_b(primitives.value).value);

  testsuite_assorted_primitives_set_o_b(primitives.value,
                                        djinni_optional_bool_make(false));
  ASSERT_TRUE(
      testsuite_assorted_primitives_get_o_b(primitives.value).has_value);
  ASSERT_FALSE(testsuite_assorted_primitives_get_o_b(primitives.value).value);

  testsuite_assorted_primitives_set_o_b(primitives.value,
                                        djinni_optional_bool_empty());
  ASSERT_FALSE(
      testsuite_assorted_primitives_get_o_b(primitives.value).has_value);

  // Test optional int8_t
  ASSERT_TRUE(
      testsuite_assorted_primitives_get_o_eight(primitives.value).has_value);
  ASSERT_EQ(8,
            testsuite_assorted_primitives_get_o_eight(primitives.value).value);

  testsuite_assorted_primitives_set_o_eight(primitives.value,
                                            djinni_optional_int8_t_make(16));
  ASSERT_TRUE(
      testsuite_assorted_primitives_get_o_eight(primitives.value).has_value);
  ASSERT_EQ(16,
            testsuite_assorted_primitives_get_o_eight(primitives.value).value);

  testsuite_assorted_primitives_set_o_eight(primitives.value,
                                            djinni_optional_int8_t_empty());
  ASSERT_FALSE(
      testsuite_assorted_primitives_get_o_eight(primitives.value).has_value);
  ASSERT_EQ(0,
            testsuite_assorted_primitives_get_o_eight(primitives.value).value);

  // Test optional int16_t
  ASSERT_TRUE(
      testsuite_assorted_primitives_get_o_sixteen(primitives.value).has_value);
  ASSERT_EQ(
      16, testsuite_assorted_primitives_get_o_sixteen(primitives.value).value);

  testsuite_assorted_primitives_set_o_sixteen(
      primitives.value, djinni_optional_int16_t_make(4343));
  ASSERT_TRUE(
      testsuite_assorted_primitives_get_o_sixteen(primitives.value).has_value);
  ASSERT_EQ(
      4343,
      testsuite_assorted_primitives_get_o_sixteen(primitives.value).value);

  testsuite_assorted_primitives_set_o_sixteen(primitives.value,
                                              djinni_optional_int16_t_empty());
  ASSERT_FALSE(
      testsuite_assorted_primitives_get_o_sixteen(primitives.value).has_value);

  // Test optional int32_t
  ASSERT_TRUE(testsuite_assorted_primitives_get_o_thirtytwo(primitives.value)
                  .has_value);
  ASSERT_EQ(
      4242,
      testsuite_assorted_primitives_get_o_thirtytwo(primitives.value).value);

  testsuite_assorted_primitives_set_o_thirtytwo(
      primitives.value, djinni_optional_int32_t_make(6000000));
  ASSERT_TRUE(testsuite_assorted_primitives_get_o_thirtytwo(primitives.value)
                  .has_value);
  ASSERT_EQ(
      6000000,
      testsuite_assorted_primitives_get_o_thirtytwo(primitives.value).value);

  testsuite_assorted_primitives_set_o_thirtytwo(
      primitives.value, djinni_optional_int32_t_empty());
  ASSERT_FALSE(testsuite_assorted_primitives_get_o_thirtytwo(primitives.value)
                   .has_value);

  // Test optional int64_t
  ASSERT_TRUE(testsuite_assorted_primitives_get_o_sixtyfour(primitives.value)
                  .has_value);
  ASSERT_EQ(
      3000000,
      testsuite_assorted_primitives_get_o_sixtyfour(primitives.value).value);

  testsuite_assorted_primitives_set_o_sixtyfour(
      primitives.value, djinni_optional_int64_t_make(999999999990));
  ASSERT_TRUE(testsuite_assorted_primitives_get_o_sixtyfour(primitives.value)
                  .has_value);
  ASSERT_EQ(
      999999999990,
      testsuite_assorted_primitives_get_o_sixtyfour(primitives.value).value);

  testsuite_assorted_primitives_set_o_sixtyfour(
      primitives.value, djinni_optional_int64_t_empty());
  ASSERT_FALSE(testsuite_assorted_primitives_get_o_sixtyfour(primitives.value)
                   .has_value);

  // Test optional float
  ASSERT_TRUE(testsuite_assorted_primitives_get_o_fthirtytwo(primitives.value)
                  .has_value);
  ASSERT_EQ(
      32.5f,
      testsuite_assorted_primitives_get_o_fthirtytwo(primitives.value).value);

  testsuite_assorted_primitives_set_o_fthirtytwo(
      primitives.value, djinni_optional_float_make(64.5f));
  ASSERT_TRUE(testsuite_assorted_primitives_get_o_fthirtytwo(primitives.value)
                  .has_value);
  ASSERT_EQ(
      64.5f,
      testsuite_assorted_primitives_get_o_fthirtytwo(primitives.value).value);

  testsuite_assorted_primitives_set_o_fthirtytwo(primitives.value,
                                                 djinni_optional_float_empty());
  ASSERT_FALSE(testsuite_assorted_primitives_get_o_fthirtytwo(primitives.value)
                   .has_value);

  // Test optional double
  ASSERT_TRUE(testsuite_assorted_primitives_get_o_fsixtyfour(primitives.value)
                  .has_value);
  ASSERT_EQ(
      6482000.5,
      testsuite_assorted_primitives_get_o_fsixtyfour(primitives.value).value);

  testsuite_assorted_primitives_set_o_fsixtyfour(
      primitives.value, djinni_optional_double_make(0.00001));
  ASSERT_TRUE(testsuite_assorted_primitives_get_o_fsixtyfour(primitives.value)
                  .has_value);
  ASSERT_EQ(
      0.00001,
      testsuite_assorted_primitives_get_o_fsixtyfour(primitives.value).value);

  testsuite_assorted_primitives_set_o_fsixtyfour(
      primitives.value, djinni_optional_double_empty());
  ASSERT_FALSE(testsuite_assorted_primitives_get_o_fsixtyfour(primitives.value)
                   .has_value);
}

TEST(DjinniCAPI, supportsRefCountedValues) {
  auto content = CRef(djinni_string_create("Hello World", 11));
  auto record =
      CRef(testsuite_client_returned_record_create(0, content.value, nullptr));
  auto returnedContent =
      CRef(testsuite_client_returned_record_get_content(record.value));
  ASSERT_EQ(std::string("Hello World"),
            std::string(djinni_string_get_data(returnedContent.value)));
}

TEST(DjinniCAPI, supportsOptionalRefCountedValues) {
  auto content = CRef(djinni_string_create("Hello World", 11));
  auto record =
      CRef(testsuite_client_returned_record_create(0, content.value, nullptr));
  auto returnedMisc =
      CRef(testsuite_client_returned_record_get_misc(record.value));

  ASSERT_TRUE(returnedMisc.value == nullptr);
  testsuite_client_returned_record_set_misc(record.value, content.value);

  auto returnedMisc2 =
      CRef(testsuite_client_returned_record_get_misc(record.value));
  ASSERT_EQ(std::string("Hello World"),
            std::string(djinni_string_get_data(returnedMisc2.value)));
}

TEST(DjinniCAPI, supportsListOfPrimitiveValues) {
  auto list = CRef(djinni_array_create(2));
  auto entry1 = CRef(djinni_number_int64_create(42));
  auto entry2 = CRef(djinni_number_int64_create(10000));
  djinni_array_set_value(list.value, 0, entry1.value);
  djinni_array_set_value(list.value, 1, entry2.value);

  auto primitiveList =
      CRef(testsuite_primitive_list_create(list.value, nullptr));

  auto returnedList =
      CRef(testsuite_primitive_list_get_list(primitiveList.value));

  auto returnedLength = djinni_array_get_length(returnedList.value);

  ASSERT_EQ(2, returnedLength);

  auto returnedEntry1 = CRef(djinni_array_get_value(returnedList.value, 0));
  auto returnedEntry2 = CRef(djinni_array_get_value(returnedList.value, 1));

  ASSERT_EQ(42, djinni_number_get_int64(returnedEntry1.value));
  ASSERT_EQ(10000, djinni_number_get_int64(returnedEntry2.value));
}

TEST(DjinniCAPI, supportsListOfOptionalPrimitiveValues) {
  auto emptyList = CRef(djinni_array_create(0));
  auto optionalList = CRef(djinni_array_create(2));
  auto entry2 = CRef(djinni_number_int64_create(10000));
  djinni_array_set_value(optionalList.value, 0, nullptr);
  djinni_array_set_value(optionalList.value, 1, entry2.value);

  auto primitiveList = CRef(
      testsuite_primitive_list_create(emptyList.value, optionalList.value));

  auto returnedList =
      CRef(testsuite_primitive_list_get_optional_list(primitiveList.value));

  auto returnedLength = djinni_array_get_length(returnedList.value);

  ASSERT_EQ(2, returnedLength);

  auto returnedEntry1 = CRef(djinni_array_get_value(returnedList.value, 0));
  auto returnedEntry2 = CRef(djinni_array_get_value(returnedList.value, 1));

  ASSERT_TRUE(returnedEntry1.value == nullptr);
  ASSERT_EQ(10000, djinni_number_get_int64(returnedEntry2.value));
}

TEST(DjinniCAPI, supportsMap) {
  auto map = CRef(djinni_keyval_array_create(2));
  auto imap = CRef(djinni_keyval_array_create(1));

  auto entry1Key = CRef(djinni_string_create("key1", 4));
  auto entry1Value = CRef(djinni_number_int64_create(42));
  auto entry2Key = CRef(djinni_string_create("key2", 4));
  auto entry2Value = CRef(djinni_number_int64_create(1));

  djinni_keyval_array_set_entry(map.value, 0, entry1Key.value,
                                entry1Value.value);
  djinni_keyval_array_set_entry(map.value, 1, entry2Key.value,
                                entry2Value.value);

  auto imapEntry1Key = CRef(djinni_number_int64_create(10));
  auto imapEntry1Value = CRef(djinni_number_int64_create(20));
  djinni_keyval_array_set_entry(imap.value, 0, imapEntry1Key.value,
                                imapEntry1Value.value);

  auto mapRecord = CRef(testsuite_map_record_create(map.value, imap.value));

  auto collectedMap = CRef(testsuite_map_record_get_map(mapRecord.value));
  auto collectedImap = CRef(testsuite_map_record_get_imap(mapRecord.value));

  ASSERT_EQ(2, djinni_keyval_array_get_length(collectedMap.value));
  ASSERT_EQ(1, djinni_keyval_array_get_length(collectedImap.value));

  auto collectedEntry1Key =
      CRef(djinni_keyval_array_get_key(collectedMap.value, 0));
  auto collectedEntry1Value =
      CRef(djinni_keyval_array_get_value(collectedMap.value, 0));
  auto collectedEntry2Key =
      CRef(djinni_keyval_array_get_key(collectedMap.value, 1));
  auto collectedEntry2Value =
      CRef(djinni_keyval_array_get_value(collectedMap.value, 1));

  // Ordering is not guaranteed because the backing store used an
  // std::unordered_map. This handles the case where the returned result is in
  // the reverse order
  if (djinni_number_get_int64(collectedEntry1Value.value) == 1) {
    ASSERT_EQ(std::string("key2"),
              std::string(djinni_string_get_data(collectedEntry1Key.value),
                          djinni_string_get_length(collectedEntry1Key.value)));
    ASSERT_EQ(1, djinni_number_get_int64(collectedEntry1Value.value));

    ASSERT_EQ(std::string("key1"),
              std::string(djinni_string_get_data(collectedEntry2Key.value),
                          djinni_string_get_length(collectedEntry2Key.value)));
    ASSERT_EQ(42, djinni_number_get_int64(collectedEntry2Value.value));
  } else {

    ASSERT_EQ(std::string("key1"),
              std::string(djinni_string_get_data(collectedEntry1Key.value),
                          djinni_string_get_length(collectedEntry1Key.value)));

    ASSERT_EQ(42, djinni_number_get_int64(collectedEntry1Value.value));

    ASSERT_EQ(std::string("key2"),
              std::string(djinni_string_get_data(collectedEntry2Key.value),
                          djinni_string_get_length(collectedEntry2Key.value)));

    ASSERT_EQ(1, djinni_number_get_int64(collectedEntry2Value.value));
  }

  auto collectedImapKey =
      CRef(djinni_keyval_array_get_key(collectedImap.value, 0));
  auto collectedImapValue =
      CRef(djinni_keyval_array_get_value(collectedImap.value, 0));

  ASSERT_EQ(10, djinni_number_get_int64(collectedImapKey.value));
  ASSERT_EQ(20, djinni_number_get_int64(collectedImapValue.value));
}

TEST(DjinniCAPI, supportsEnum) {}

TEST(DjinniCAPI, supportsBinaryRef) {}

TEST(DjinniCAPI, supportsInterface) {}

TEST(DjinniCAPI, supportsCallingStaticMethodsOnInterface) {}

} // namespace djinni
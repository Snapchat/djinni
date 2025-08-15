#include "assorted_primitives.h"
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

TEST(DjinniCAPI, supportsRefCountedValues) {}

TEST(DjinniCAPI, supportsOptionalRefCountedvalues) {}

TEST(DjinniCAPI, supportsListOfPrimitiveValues) {}

TEST(DjinniCAPI, supportsListOfOptionalPrimitiveValues) {}

TEST(DjinniCAPI, supportsListOfRefCountedValues) {}

TEST(DjinniCAPI, supportsMap) {}

TEST(DjinniCAPI, supportsEnum) {}

TEST(DjinniCAPI, supportsBinaryRef) {}

TEST(DjinniCAPI, supportsInterface) {}

TEST(DjinniCAPI, supportsCallingStaticMethodsOnInterface) {}

} // namespace djinni
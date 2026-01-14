#include "DataRefTest.h"
#include "Future.hpp"
#include "assorted_primitives.h"
#include "client_interface.h"
#include "client_returned_record.h"
#include "constant_with_enum.h"
#include "constants.h"
#include "enum_usage_record.h"
#include "map_record.h"
#include "primitive_list.h"
#include "proto/cpp/test.pb.h"
#include "proto_tests.h"
#include "test_helpers.h"
#include "test_outcome.h"
#include "test_lazy.h"
#include "simple_object.h"
#include "gtest/gtest.h"
#include <memory>

namespace djinni {

template <typename T> struct CRef {
  T value;
  CRef(T value) : value(value) {}

  ~CRef() { djinni_ref_release(value); }
};

TEST(DjinniCAPI, supportsPrimitiveValues) {
  auto primitives = CRef(testsuite_assorted_primitives_new(
      true, 8, 4242, 3000000, 99999999999, 32.5f, 6482000.5,
      djinni_optional_bool_empty(), djinni_optional_int8_empty(),
      djinni_optional_int16_empty(), djinni_optional_int32_empty(),
      djinni_optional_int64_empty(), djinni_optional_float_empty(),
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
  auto primitives = CRef(testsuite_assorted_primitives_new(
      false, 0, 0, 0, 0, 0.0f, 0.0, djinni_optional_bool_make(true),
      djinni_optional_int8_make(8), djinni_optional_int16_make(16),
      djinni_optional_int32_make(4242), djinni_optional_int64_make(3000000),
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
                                            djinni_optional_int8_make(16));
  ASSERT_TRUE(
      testsuite_assorted_primitives_get_o_eight(primitives.value).has_value);
  ASSERT_EQ(16,
            testsuite_assorted_primitives_get_o_eight(primitives.value).value);

  testsuite_assorted_primitives_set_o_eight(primitives.value,
                                            djinni_optional_int8_empty());
  ASSERT_FALSE(
      testsuite_assorted_primitives_get_o_eight(primitives.value).has_value);
  ASSERT_EQ(0,
            testsuite_assorted_primitives_get_o_eight(primitives.value).value);

  // Test optional int16_t
  ASSERT_TRUE(
      testsuite_assorted_primitives_get_o_sixteen(primitives.value).has_value);
  ASSERT_EQ(
      16, testsuite_assorted_primitives_get_o_sixteen(primitives.value).value);

  testsuite_assorted_primitives_set_o_sixteen(primitives.value,
                                              djinni_optional_int16_make(4343));
  ASSERT_TRUE(
      testsuite_assorted_primitives_get_o_sixteen(primitives.value).has_value);
  ASSERT_EQ(
      4343,
      testsuite_assorted_primitives_get_o_sixteen(primitives.value).value);

  testsuite_assorted_primitives_set_o_sixteen(primitives.value,
                                              djinni_optional_int16_empty());
  ASSERT_FALSE(
      testsuite_assorted_primitives_get_o_sixteen(primitives.value).has_value);

  // Test optional int32_t
  ASSERT_TRUE(testsuite_assorted_primitives_get_o_thirtytwo(primitives.value)
                  .has_value);
  ASSERT_EQ(
      4242,
      testsuite_assorted_primitives_get_o_thirtytwo(primitives.value).value);

  testsuite_assorted_primitives_set_o_thirtytwo(
      primitives.value, djinni_optional_int32_make(6000000));
  ASSERT_TRUE(testsuite_assorted_primitives_get_o_thirtytwo(primitives.value)
                  .has_value);
  ASSERT_EQ(
      6000000,
      testsuite_assorted_primitives_get_o_thirtytwo(primitives.value).value);

  testsuite_assorted_primitives_set_o_thirtytwo(primitives.value,
                                                djinni_optional_int32_empty());
  ASSERT_FALSE(testsuite_assorted_primitives_get_o_thirtytwo(primitives.value)
                   .has_value);

  // Test optional int64_t
  ASSERT_TRUE(testsuite_assorted_primitives_get_o_sixtyfour(primitives.value)
                  .has_value);
  ASSERT_EQ(
      3000000,
      testsuite_assorted_primitives_get_o_sixtyfour(primitives.value).value);

  testsuite_assorted_primitives_set_o_sixtyfour(
      primitives.value, djinni_optional_int64_make(999999999990));
  ASSERT_TRUE(testsuite_assorted_primitives_get_o_sixtyfour(primitives.value)
                  .has_value);
  ASSERT_EQ(
      999999999990,
      testsuite_assorted_primitives_get_o_sixtyfour(primitives.value).value);

  testsuite_assorted_primitives_set_o_sixtyfour(primitives.value,
                                                djinni_optional_int64_empty());
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
  auto content = CRef(djinni_string_new("Hello World", 11));
  auto record =
      CRef(testsuite_client_returned_record_new(0, content.value, nullptr));
  auto returnedContent =
      CRef(testsuite_client_returned_record_get_content(record.value));
  ASSERT_EQ(std::string("Hello World"),
            std::string(djinni_string_get_data(returnedContent.value)));
}

TEST(DjinniCAPI, supportsOptionalRefCountedValues) {
  auto content = CRef(djinni_string_new("Hello World", 11));
  auto record =
      CRef(testsuite_client_returned_record_new(0, content.value, nullptr));
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
  auto list = CRef(djinni_array_new(2));
  auto entry1 = CRef(djinni_number_int64_new(42));
  auto entry2 = CRef(djinni_number_int64_new(10000));
  djinni_array_set_value(list.value, 0, entry1.value);
  djinni_array_set_value(list.value, 1, entry2.value);

  auto primitiveList = CRef(testsuite_primitive_list_new(list.value, nullptr));

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
  auto emptyList = CRef(djinni_array_new(0));
  auto optionalList = CRef(djinni_array_new(2));
  auto entry2 = CRef(djinni_number_int64_new(10000));
  djinni_array_set_value(optionalList.value, 0, nullptr);
  djinni_array_set_value(optionalList.value, 1, entry2.value);

  auto primitiveList =
      CRef(testsuite_primitive_list_new(emptyList.value, optionalList.value));

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
  auto map = CRef(djinni_keyval_array_new(2));
  auto imap = CRef(djinni_keyval_array_new(1));

  auto entry1Key = CRef(djinni_string_new("key1", 4));
  auto entry1Value = CRef(djinni_number_int64_new(42));
  auto entry2Key = CRef(djinni_string_new("key2", 4));
  auto entry2Value = CRef(djinni_number_int64_new(1));

  djinni_keyval_array_set_entry(map.value, 0, entry1Key.value,
                                entry1Value.value);
  djinni_keyval_array_set_entry(map.value, 1, entry2Key.value,
                                entry2Value.value);

  auto imapEntry1Key = CRef(djinni_number_int64_new(10));
  auto imapEntry1Value = CRef(djinni_number_int64_new(20));
  djinni_keyval_array_set_entry(imap.value, 0, imapEntry1Key.value,
                                imapEntry1Value.value);

  auto mapRecord = CRef(testsuite_map_record_new(map.value, imap.value));

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

TEST(DjinniCAPI, supportsEnum) {
  auto optionalEnum = CRef(djinni_number_uint64_new(testsuite_color_VIOLET));
  auto list = CRef(djinni_array_new(1));
  auto listEntry = CRef(djinni_number_uint64_new(testsuite_color_ORANGE));
  djinni_array_set_value(list.value, 0, listEntry.value);

  auto map = CRef(djinni_keyval_array_new(0));

  auto record = CRef(
      testsuite_enum_usage_record_new(testsuite_color_BLUE, optionalEnum.value,
                                      list.value, list.value, map.value));

  ASSERT_EQ(testsuite_color_BLUE,
            testsuite_enum_usage_record_get_e(record.value));

  auto returnedOptional = CRef(testsuite_enum_usage_record_get_o(record.value));

  ASSERT_EQ(testsuite_color_VIOLET,
            djinni_number_get_uint64(returnedOptional.value));

  auto returnedList = CRef(testsuite_enum_usage_record_get_l(record.value));

  ASSERT_EQ(1, djinni_array_get_length(returnedList.value));

  auto entry = CRef(djinni_array_get_value(returnedList.value, 0));

  ASSERT_EQ(testsuite_color_ORANGE, djinni_number_get_uint64(entry.value));
}

struct DataHolder {
  std::vector<uint8_t> data;
  bool deallocCalled = false;
};

static void data_holder_free_callback(uint8_t *data, size_t length,
                                      void *opaque) {
  reinterpret_cast<DataHolder *>(opaque)->deallocCalled = true;
}

TEST(DjinniCAPI, supportsBinaryRef) {
  auto ref = CRef(testsuite_DataRefTest_create());

  auto receivedData = CRef(testsuite_DataRefTest_generateData(ref.value));

  auto length = djinni_binary_get_length(receivedData.value);
  ASSERT_EQ(4, length);

  ASSERT_EQ(0, djinni_binary_get_data(receivedData.value)[0]);
  ASSERT_EQ(1, djinni_binary_get_data(receivedData.value)[1]);
  ASSERT_EQ(2, djinni_binary_get_data(receivedData.value)[2]);
  ASSERT_EQ(3, djinni_binary_get_data(receivedData.value)[3]);

  auto dataHolder = std::make_unique<DataHolder>();
  dataHolder->data.resize(3);
  dataHolder->data[0] = 1;
  dataHolder->data[1] = 10;
  dataHolder->data[2] = 100;

  {
    auto input = CRef(djinni_binary_new_with_bytes(
        dataHolder->data.data(), dataHolder->data.size(), dataHolder.get(),
        &data_holder_free_callback));
    auto received =
        CRef(testsuite_DataRefTest_sendDataView(ref.value, input.value));

    ASSERT_EQ(3, djinni_binary_get_length(received.value));
    ASSERT_EQ(1, djinni_binary_get_data(received.value)[0]);
    ASSERT_EQ(10, djinni_binary_get_data(received.value)[1]);
    ASSERT_EQ(100, djinni_binary_get_data(received.value)[2]);

    testsuite_DataRefTest_sendData(ref.value, input.value);
  }

  ASSERT_FALSE(dataHolder->deallocCalled);
  // Create another one and send it.Because it's received as an actual DataRef,
  // the C++ will have retained the data ref and then release it to set the new
  // one
  auto newInput = CRef(djinni_binary_new_with_bytes_copy(nullptr, 0));
  testsuite_DataRefTest_sendData(ref.value, newInput.value);
  ASSERT_TRUE(dataHolder->deallocCalled);
}

static testsuite_client_returned_record_ref getRecord(void *opaque,
                                                      int64_t recordId,
                                                      djinni_string_ref content,
                                                      djinni_string_ref misc) {
  return testsuite_client_returned_record_new(recordId, content, misc);
}

TEST(DjinniCAPI, supportsInterface) {
  testsuite_client_interface_method_defs methodDefs = {0};
  methodDefs.get_record = &getRecord;

  auto proxyClass =
      CRef(testsuite_client_interface_proxy_class_new(&methodDefs, nullptr));
  auto proxy = CRef(testsuite_client_interface_new(proxyClass.value, nullptr));

  auto content = CRef(djinni_string_new("Hello World", 11));

  auto record = CRef(testsuite_client_interface_get_record(
      proxy.value, 42, content.value, nullptr));

  ASSERT_EQ(42, testsuite_client_returned_record_get_record_id(record.value));
  auto recordContent =
      CRef(testsuite_client_returned_record_get_content(record.value));
  ASSERT_EQ(std::string("Hello World"),
            std::string(djinni_string_get_data(recordContent.value),
                        djinni_string_get_length(recordContent.value)));

  // Should also work when passed to an interface taking C++
  testsuite_test_helpers_check_client_interface_ascii(proxy.value);
  testsuite_test_helpers_check_client_interface_nonascii(proxy.value);
}

static void opaqueDeallocator(void *opaque) {
  ++(*reinterpret_cast<int *>(opaque));
}

TEST(DjinniCAPI, interfaceDeallocatesOpaque) {
  int deallocateCount = 0;
  testsuite_client_interface_method_defs methodDefs = {0};

  auto proxyClass = testsuite_client_interface_proxy_class_new(
      &methodDefs, &opaqueDeallocator);
  auto proxy1 = testsuite_client_interface_new(proxyClass, &deallocateCount);
  auto proxy2 = testsuite_client_interface_new(proxyClass, &deallocateCount);

  djinni_ref_release(proxyClass);

  ASSERT_EQ(0, deallocateCount);

  djinni_ref_release(proxy1);

  ASSERT_EQ(1, deallocateCount);

  djinni_ref_release(proxy2);

  ASSERT_EQ(2, deallocateCount);
}

TEST(DjinniCAPI, supportsProto) {
  ::djinni::test::AddressBook x;
  auto *p1 = x.add_people();
  p1->set_id(1);
  p1->set_name("Hello");
  auto *p2 = x.add_people();
  p2->set_id(2);
  p2->set_name("World");

  std::string pb;
  x.SerializeToString(&pb);

  auto pbBytes = CRef(djinni_binary_new_with_bytes_copy(
      reinterpret_cast<const uint8_t *>(pb.data()), pb.size()));

  auto vec = CRef(testsuite_proto_tests_protoToStrings(pbBytes.value));

  ASSERT_EQ(2, djinni_array_get_length(vec.value));

  auto retrievedName1 = CRef(djinni_array_get_value(vec.value, 0));
  auto retrievedName2 = CRef(djinni_array_get_value(vec.value, 1));

  ASSERT_EQ(std::string("Hello"),
            std::string(djinni_string_get_data(retrievedName1.value)));

  ASSERT_EQ(std::string("World"),
            std::string(djinni_string_get_data(retrievedName2.value)));
}

TEST(DjinniCAPI, supportsOutcome) {
  auto result = CRef(testsuite_test_outcome_getSuccessOutcome());

  ASSERT_TRUE(djinni_outcome_is_success(result.value));
  ASSERT_FALSE(djinni_outcome_is_error(result.value));

  auto value = CRef(djinni_outcome_get_value(result.value));

  ASSERT_EQ(std::string("hello"),
            std::string(djinni_string_get_data(value.value)));

  auto failure = CRef(testsuite_test_outcome_getErrorOutcome());

  ASSERT_FALSE(djinni_outcome_is_success(failure.value));
  ASSERT_TRUE(djinni_outcome_is_error(failure.value));

  auto error = CRef(djinni_outcome_get_error(failure.value));

  ASSERT_EQ(42, djinni_number_get_int64(error.value));
}

static void handleException(void *opaque, const char *message) {
  *reinterpret_cast<std::string *>(opaque) = std::string(message);
}

TEST(DjinniCAPI, canCatchExceptions) {
  std::string errorMessage;

  djinni_exception_handler handler;
  handler.opaque = &errorMessage;
  handler.callback = &handleException;

  djinni_exception_handler_push(&handler);
  auto result = CRef(testsuite_test_helpers_async_early_throw());
  djinni_exception_handler_pop();

  ASSERT_EQ(std::string("error"), errorMessage);
}

TEST(DjinniCAPI, supportsNestedExceptionHandler) {

  std::string errorMessage;
  std::string errorMessageInner;

  djinni_exception_handler handler;
  handler.opaque = &errorMessage;
  handler.callback = &handleException;

  djinni_exception_handler_push(&handler);
  {
    djinni_exception_handler nestedHandler;
    nestedHandler.opaque = &errorMessageInner;
    nestedHandler.callback = &handleException;
    djinni_exception_handler_push(&nestedHandler);
    auto result = CRef(testsuite_test_helpers_async_early_throw());

    djinni_exception_handler_pop();

    ASSERT_EQ(std::string(""), errorMessage);
    ASSERT_EQ(std::string("error"), errorMessageInner);
  }

  auto result = CRef(testsuite_test_helpers_async_early_throw());
  ASSERT_EQ(std::string("error"), errorMessage);
  ASSERT_EQ(std::string("error"), errorMessageInner);

  djinni_exception_handler_pop();
}

static void futureIntCallback(void *opaque, djinni_ref value,
                              djinni_ref error) {
  auto result = djinni_number_get_int64(value);
  reinterpret_cast<::djinni::Promise<int> *>(opaque)->setValue(
      static_cast<int>(result));
}

TEST(DjinniCAPI, supportsFuture) {
  auto future = CRef(testsuite_test_helpers_get_async_result());

  ::djinni::Promise<int> promise;

  djinni_future_on_complete(future.value, reinterpret_cast<void *>(&promise),
                            nullptr, &futureIntCallback);

  auto cppFuture = promise.getFuture();
  auto result = cppFuture.get();

  ASSERT_EQ(42, result);
}

struct FutureCallbackResult {
  djinni_ref value = nullptr;
  djinni_string_ref error = nullptr;
  bool completed = false;

  ~FutureCallbackResult() {
    djinni_ref_release(value);
    djinni_ref_release(error);
  }
};

static void futureCallback(void *opaque, djinni_ref value, djinni_ref error) {
  auto result = reinterpret_cast<FutureCallbackResult *>(opaque);
  result->value = value;
  result->error = error;
  result->completed = true;
  djinni_ref_retain(value);
  djinni_ref_retain(error);
}

TEST(DjinniCAPI, supportsFutureCreatedFromC) {
  auto promise = CRef(djinni_promise_new());
  auto future = CRef(djinni_promise_get_future(promise.value));

  auto newFuture = CRef(testsuite_test_helpers_future_roundtrip(future.value));

  FutureCallbackResult result;
  djinni_future_on_complete(newFuture.value, &result, nullptr, &futureCallback);

  ASSERT_FALSE(result.completed);
  ASSERT_FALSE(result.value != nullptr);

  auto intResult = CRef(djinni_number_int64_new(42));

  djinni_promise_resolve(promise.value, intResult.value);

  ASSERT_TRUE(result.completed);
  ASSERT_TRUE(result.value != nullptr);

  ASSERT_EQ(std::string("42"),
            std::string(djinni_string_get_data(result.value)));
}

TEST(DjinniCAPI, supportsVoidFuture) {
  auto promise = CRef(djinni_promise_new());
  auto future = CRef(djinni_promise_get_future(promise.value));

  auto newFuture = CRef(testsuite_test_helpers_void_async_method(future.value));

  FutureCallbackResult result;
  djinni_future_on_complete(newFuture.value, &result, nullptr, &futureCallback);

  ASSERT_FALSE(result.completed);

  djinni_promise_resolve(promise.value, nullptr);

  ASSERT_TRUE(result.completed);
  ASSERT_TRUE(result.value == nullptr);
  ASSERT_TRUE(result.error == nullptr);
}

TEST(DjinniCAPI, supportsPropagatingErrorInFuture) {
  auto promise = CRef(djinni_promise_new());
  auto future = CRef(djinni_promise_get_future(promise.value));

  auto newFuture = CRef(testsuite_test_helpers_void_async_method(future.value));

  FutureCallbackResult result;
  djinni_future_on_complete(newFuture.value, &result, nullptr, &futureCallback);

  ASSERT_FALSE(result.completed);

  auto errorString = CRef(djinni_string_new("Error", 5));

  djinni_promise_reject(promise.value, errorString.value);

  ASSERT_TRUE(result.completed);
  ASSERT_TRUE(result.value == nullptr);
  ASSERT_TRUE(result.error != nullptr);

  ASSERT_EQ(std::string("Error"),
            std::string(djinni_string_get_data(result.error)));
}

TEST(DjinniCAPI, supportsConstants) {
  ASSERT_EQ(true, testsuite_constants_get_bool_constant());
  ASSERT_EQ(4, testsuite_constants_get_i64_constant());
  ASSERT_EQ(5.0f, testsuite_constants_get_f32_constant());
  ASSERT_EQ(5.0, testsuite_constants_get_f64_constant());

  auto opt = testsuite_constants_get_opt_f64_constant();
  ASSERT_TRUE(opt.has_value);
  ASSERT_EQ(5.0, opt.value);

  ASSERT_EQ(testsuite_constant_enum_SOME_VALUE,
            testsuite_constant_with_enum_get_const_enum());
}

TEST(DjinniCAPI, supportsConstantsRecord) {
  auto constant = CRef(testsuite_constants_get_object_constant());

  ASSERT_EQ(3, testsuite_constant_record_get_some_integer(constant.value));

  auto str = CRef(testsuite_constant_record_get_some_string(constant.value));
  ASSERT_EQ(std::string("string-constant"),
            std::string(djinni_string_get_data(str.value)));
}

TEST(DjinniCAPI, supportsLazy) {
  // Test getting lazy string from C++ and calling it
  auto lazyStr = CRef(testsuite_test_lazy_getLazyString());
  auto str = CRef(djinni_lazy_call(lazyStr.value));
  ASSERT_EQ(std::string("hello"), std::string(djinni_string_get_data(str.value)));

  // Test getting lazy int from C++ and calling it
  auto lazyInt = CRef(testsuite_test_lazy_getLazyInt());
  auto intVal = CRef(djinni_lazy_call(lazyInt.value));
  ASSERT_EQ(42, djinni_number_get_int64(intVal.value));

  // Test passing lazy string to C++
  struct StringContext {
    const char* value;
  };
  StringContext strCtx = {"world"};
  
  auto cLazyStr = CRef(djinni_lazy_make(
      [](void* ctx) -> djinni_ref {
        auto* strCtx = static_cast<StringContext*>(ctx);
        return djinni_string_new(strCtx->value, strlen(strCtx->value));
      },
      &strCtx,
      nullptr));
  
  auto result = CRef(testsuite_test_lazy_callLazyString(cLazyStr.value));
  ASSERT_EQ(std::string("world"), std::string(djinni_string_get_data(result.value)));

  // Test passing lazy int to C++
  struct IntContext {
    int64_t value;
  };
  IntContext intCtx = {123};
  
  auto cLazyInt = CRef(djinni_lazy_make(
      [](void* ctx) -> djinni_ref {
        auto* intCtx = static_cast<IntContext*>(ctx);
        return djinni_number_int64_new(intCtx->value);
      },
      &intCtx,
      nullptr));
  
  ASSERT_EQ(123, testsuite_test_lazy_callLazyInt(cLazyInt.value));
}

TEST(DjinniCAPI, supportsLazyInterface) {
  // Test getting lazy interface object from C++ and calling it
  auto lazyObj = CRef(testsuite_test_lazy_getLazyObject());
  auto obj = CRef(djinni_lazy_call(lazyObj.value));
  
  ASSERT_EQ(42, testsuite_simple_object_get_value(obj.value));
  auto name = CRef(testsuite_simple_object_get_name(obj.value));
  ASSERT_EQ(std::string("expensive"), std::string(djinni_string_get_data(name.value)));

  // Test passing lazy interface to C++
  struct ObjectContext {
    int32_t value;
    const char* name;
    int* creationCount;
  };
  int creationCount = 0;
  ObjectContext objCtx = {999, "c-created", &creationCount};
  
  auto cLazyObj = CRef(djinni_lazy_make(
      [](void* ctx) -> djinni_ref {
        auto* objCtx = static_cast<ObjectContext*>(ctx);
        (*objCtx->creationCount)++;
        auto cName = CRef(djinni_string_new(objCtx->name, strlen(objCtx->name)));
        return testsuite_test_lazy_createSimpleObject(objCtx->value, cName.value);
      },
      &objCtx,
      nullptr));
  
  // Object not created yet
  ASSERT_EQ(0, creationCount);
  
  // Call from C++ - object created on demand
  ASSERT_EQ(999, testsuite_test_lazy_callLazyObject(cLazyObj.value));
  ASSERT_EQ(1, creationCount);
  
  // Second call - object created again (not memoized in this test)
  auto objName = CRef(testsuite_test_lazy_callLazyObjectGetName(cLazyObj.value));
  ASSERT_EQ(std::string("c-created"), std::string(djinni_string_get_data(objName.value)));
  ASSERT_EQ(2, creationCount);
}

} // namespace djinni
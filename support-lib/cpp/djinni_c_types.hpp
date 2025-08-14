#pragma once

#include <atomic>
#include <chrono>
#include <string>

namespace djinni {

class Object {
public:
  Object();
  virtual ~Object();

  static void retain(Object *object);
  static void release(Object *object);

private:
  std::atomic_int _ref;

  void onRetain();
  void onRelease();
};

class String : public Object {
public:
  String(size_t length);
  ~String() override;

  const char *data() const;
  size_t length() const;

  static String *make(const char *str, size_t length);

private:
  size_t _length;
};

class ObjectArray : public Object {
public:
  ObjectArray(size_t length);
  ~ObjectArray() override;

  size_t length() const;

  Object *getObjectAtIndex(size_t index) const;
  void setObjectAtIndex(size_t index, Object *object);

  static ObjectArray *make(size_t length);

private:
  size_t _length;
};

class Binary : public Object {
public:
  using Deallocator = void (*)(uint8_t *, size_t, void *);
  Binary(uint8_t *data, size_t length, void *opaque,
         Binary::Deallocator deallocator);
  ~Binary() override;

  uint8_t *data() const;
  size_t length() const;

  static Binary *make(uint8_t *data, size_t length, void *opaque,
                      Binary::Deallocator deallocator);

private:
  uint8_t *_data;
  size_t _length;
  void *_opaque;
  Binary::Deallocator _deallocator;
};

class Number : public Object {
public:
  union Value {
    uint64_t u;
    int64_t i;
    double d;
  };

  enum class ValueType { UNSIGNED_INT, SIGNED_INT, DOUBLE };

  Number(Value value, ValueType type);
  ~Number() override;

  uint64_t toUnsignedInt() const;
  int64_t toSignedInt() const;
  double toDouble() const;

  static Number *make(Value value, ValueType type);

private:
  Value _value;
  ValueType _type;
};

template <typename T> class RecordHolder : public Object {
public:
  RecordHolder(T data) : _data(std::move(data)) {}
  ~RecordHolder() override = default;

  const T &data() const { return _data; }

  T &data() { return _data; }

private:
  T _data;
};

} // namespace djinni
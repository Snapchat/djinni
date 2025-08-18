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
  Binary(uint8_t *data, size_t length);
  ~Binary() override;

  uint8_t *data() const;
  size_t length() const;

private:
  uint8_t *_data;
  size_t _length;
};

class BinaryWithDeallocator : public Binary {
public:
  using Deallocator = void (*)(uint8_t *, size_t, void *);
  BinaryWithDeallocator(uint8_t *data, size_t length, void *opaque,
                        BinaryWithDeallocator::Deallocator deallocator);
  ~BinaryWithDeallocator() override;

  static BinaryWithDeallocator *
  make(uint8_t *data, size_t length, void *opaque,
       BinaryWithDeallocator::Deallocator deallocator);

private:
  void *_opaque;
  BinaryWithDeallocator::Deallocator _deallocator;
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

template <typename T> class InterfaceHolder : public Object {
public:
  InterfaceHolder(std::shared_ptr<T> data) : _data(std::move(data)) {}
  ~InterfaceHolder() override = default;

  const std::shared_ptr<T> &data() const { return _data; }

private:
  std::shared_ptr<T> _data;
};

template <typename T> class ProxyClass : public Object {
public:
  ProxyClass(const T &methodDefs) : _methodDefs(methodDefs) {}
  ~ProxyClass() override = default;

  const T &methodDefs() const {
    return _methodDefs;
  }
private:
  T _methodDefs;
};

} // namespace djinni
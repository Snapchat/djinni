#include "djinni_c_types.hpp"

namespace djinni {

constexpr size_t alignUp(size_t size, size_t alignment) {
  return (size + alignment - 1) & ~(alignment - 1);
}

template <typename T, typename ValueType> struct ArrayAllocator {
  template <typename... Args> T *allocate(size_t size, Args &&...args) {
    std::allocator<uint8_t> allocator;

    auto allocSize =
        alignUp(sizeof(T), alignof(ValueType)) + (sizeof(ValueType) * size);

    auto *arrayRegion = allocator.allocate(allocSize);

    new (arrayRegion)(T)(std::forward<Args>(args)...);

    return reinterpret_cast<T *>(arrayRegion);
  }

  constexpr inline ValueType *getContainerStartPtr(T *object) {
    return reinterpret_cast<ValueType *>(
        reinterpret_cast<uint8_t *>(object) +
        alignUp(sizeof(T), alignof(ValueType)));
  }

  constexpr inline const ValueType *getContainerStartPtr(const T *object) {
    return reinterpret_cast<const ValueType *>(
        reinterpret_cast<const uint8_t *>(object) +
        alignUp(sizeof(T), alignof(ValueType)));
  }
};

static Object *swapObject(Object *prevValue, Object *newValue) {
  Object::retain(newValue);
  Object::release(prevValue);

  return newValue;
}

Object::Object() : _ref(1) {}

Object::~Object() = default;

void Object::onRetain() { _ref++; }

void Object::onRelease() {
  if (--_ref == 0) {
    delete this;
  }
}

void Object::retain(Object *obj) {
  if (obj != nullptr) {
    obj->onRetain();
  }
}

void Object::release(Object *obj) {
  if (obj != nullptr) {
    obj->onRelease();
  }
}

using StringAllocator = ArrayAllocator<String, char>;

String::String(size_t length) : _length(length) {}
String::~String() = default;

const char *String::data() const {
  StringAllocator allocator;
  return allocator.getContainerStartPtr(this);
}

size_t String::String::length() const { return _length; }

String *String::make(const char *str, size_t length) {
  StringAllocator allocator;
  auto *output = allocator.allocate(length, length);
  auto *data = allocator.getContainerStartPtr(output);
  for (size_t i = 0; i < length; i++) {
    data[i] = str[i];
  }
  data[length] = 0;

  return output;
}

using ObjectArrayAllocator = ArrayAllocator<ObjectArray, Object *>;

ObjectArray::ObjectArray(size_t length) : _length(length) {}

ObjectArray::~ObjectArray() {
  ObjectArrayAllocator allocator;
  auto **data = allocator.getContainerStartPtr(this);
  for (size_t i = 0; i < _length; i++) {
    Object::release(data[i]);
  }
}

size_t ObjectArray::length() const { return _length; }

Object *ObjectArray::getObjectAtIndex(size_t index) const {
  ObjectArrayAllocator allocator;
  Object *const *data = allocator.getContainerStartPtr(this);
  auto *object = data[index];
  Object::retain(object);
  return object;
}

void ObjectArray::setObjectAtIndex(Object *object, size_t index) {
  ObjectArrayAllocator allocator;
  Object **data = allocator.getContainerStartPtr(this);

  Object::retain(object);
  Object::release(data[index]);
  data[index] = object;
}

ObjectArray *ObjectArray::make(size_t length) {
  ObjectArrayAllocator allocator;
  auto *output = allocator.allocate(length, length);

  std::memset(allocator.getContainerStartPtr(output), 0, length);

  return output;
}

Binary::Binary(uint8_t *data, size_t length, void *opaque,
               Binary::Deallocator deallocator)
    : _data(data), _length(length), _opaque(opaque), _deallocator(deallocator) {
}

Binary::~Binary() {
  if (_deallocator) {
    _deallocator(_data, _length, _opaque);
  }
}

uint8_t *Binary::data() const { return _data; }

size_t Binary::length() const { return _length; }

Binary *Binary::make(uint8_t *data, size_t length, void *opaque,
                     Binary::Deallocator deallocator) {
  return new Binary(data, length, opaque, deallocator);
}

Number::Number(Number::Value value, Number::ValueType type)
    : _value(value), _type(type) {}
Number::~Number() = default;

uint64_t Number::toUnsignedInt() const {
  switch (_type) {
  case Number::ValueType::UNSIGNED_INT:
    return _value.u;
  case Number::ValueType::SIGNED_INT:
    return static_cast<uint64_t>(_value.i);
  case Number::ValueType::DOUBLE:
    return static_cast<uint64_t>(_value.d);
  }
}

int64_t Number::toSignedInt() const {
  switch (_type) {
  case Number::ValueType::UNSIGNED_INT:
    return static_cast<int64_t>(_value.u);
  case Number::ValueType::SIGNED_INT:
    return _value.i;
  case Number::ValueType::DOUBLE:
    return static_cast<int64_t>(_value.d);
  }
}

double Number::toDouble() const {
  switch (_type) {
  case Number::ValueType::UNSIGNED_INT:
    return static_cast<double>(_value.u);
  case Number::ValueType::SIGNED_INT:
    return static_cast<double>(_value.i);
  case Number::ValueType::DOUBLE:
    return _value.d;
  }
}

Number *Number::make(Number::Value value, Number::ValueType type) {
  return new Number(value, type);
}

} // namespace djinni
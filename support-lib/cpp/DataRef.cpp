#include "DataRef.hpp"

#if DATAREF_JNI
  #include "djinni_support.hpp"
  #include <variant>
#endif

#if DATAREF_WASM
  #include "djinni_wasm.hpp"
#endif

namespace snapchat {
namespace djinni {

#if DATAREF_JNI
class DataRefJNI : public DataRef::Impl {
    struct ByteBufferClassInfo {
        ::djinni::GlobalRef<jclass> classObject;
        jmethodID allocateDirect;

        ByteBufferClassInfo() {
            classObject = ::djinni::jniFindClass("java/nio/ByteBuffer");
            assert(classObject != nullptr);
            allocateDirect =
                ::djinni::jniGetStaticMethodID(classObject.get(), "allocateDirect", "(I)Ljava/nio/ByteBuffer;");
            assert(allocateDirect != nullptr);
        }
    };
    struct BufferClassInfo {
        jmethodID isReadOnly;

        BufferClassInfo() {
            auto jcls = ::djinni::jniFindClass("java/nio/Buffer");
            assert(jcls != nullptr);
            isReadOnly = ::djinni::jniGetMethodID(jcls.get(), "isReadOnly", "()Z");
            assert(isReadOnly != nullptr);
        }
    };
    struct NativeObjectManagerClassInfo {
        ::djinni::GlobalRef<jclass> classObject;
        jmethodID registerMethodId;

        NativeObjectManagerClassInfo() {
            classObject = ::djinni::jniFindClass("com/snapchat/djinni/NativeObjectManager");
            assert(classObject != nullptr);
            registerMethodId = ::djinni::jniGetStaticMethodID(
                classObject.get(), "register", "(Ljava/lang/Object;Ljava/lang/Class;J)V");
            assert(registerMethodId != nullptr);
        }
    };
    struct DataRefHelperClassInfo {
        ::djinni::GlobalRef<jclass> classObject;

        DataRefHelperClassInfo() {
            classObject = ::djinni::jniFindClass("com/snapchat/djinni/DataRefHelper");
            assert(classObject != nullptr);
        }
    };

public:
    using DataObj = std::variant<std::vector<uint8_t>, std::string>;

    // create an empty buffer from c++
    explicit DataRefJNI(size_t len) {
        allocate(len);
    }
    // wrap a ByteBuffer object from java
    explicit DataRefJNI(jobject data) {
        auto* env = ::djinni::jniGetThreadEnv();
        _data = {env, data};
        // call ByteBuffer.isReadOnly() to determine mutability
        _readonly = env->CallBooleanMethod(_data.get(), ::djinni::JniClass<BufferClassInfo>::get().isReadOnly) != 0;
        ::djinni::jniExceptionCheck(env);
        auto len = env->GetDirectBufferCapacity(_data.get());
        assert(len != -1); // GetDirectBufferCapacity() returns -1 when the ByteBuffer is not direct
        _len = static_cast<size_t>(len);
        _buf = reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(_data.get()));
    }
    // take over a std::vector's buffer without copying it
    explicit DataRefJNI(std::vector<uint8_t>&& vec) {
        // NewDirectByteBuffer() does not accept 0 size, but 0 is a valid
        // argument for ByteBuffer.allocateDirect().
        if (!vec.empty()) {
            takeOver(std::move(vec));
        } else {
            allocate(0);
        }
    }
    // take over a std::string's buffer without copying it
    explicit DataRefJNI(std::string&& str) {
        if (!str.empty()) {
            takeOver(std::move(str));
        } else {
            allocate(0);
        }
    }

    DataRefJNI(const DataRefJNI&) = delete;

    const uint8_t* buf() const override {
        return _buf;
    }
    size_t len() const override {
        return _len;
    }
    uint8_t* mutableBuf() override {
        return _readonly ? nullptr : _buf;
    }

    PlatformObject platformObj() const override {
        return _data.get();
    }

private:
    ::djinni::GlobalRef<jobject> _data;
    bool _readonly;
    size_t _len;
    uint8_t* _buf;

    void allocate(size_t len) {
        auto* env = ::djinni::jniGetThreadEnv();
        // call Java's ByteBuffer.allocateDirect() to create a direct buffer owned by the jobject
        const auto& classInfo = ::djinni::JniClass<ByteBufferClassInfo>::get();
        ::djinni::LocalRef<jobject> localData{
            env, env->CallStaticObjectMethod(classInfo.classObject.get(), classInfo.allocateDirect, len)};
        ::djinni::jniExceptionCheck(env);
        // convert the local ref to a global ref so we can keep it
        _data = {env, localData.get()};
        _readonly = false;
        // we can store the buffer pointer and length because direct buffers won't move
        _len = len;
        _buf = reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(_data.get()));
    }

    template<typename T>
    void takeOver(T&& obj) {
        auto* env = ::djinni::jniGetThreadEnv();

        // move input object to a new heap object
        auto p = std::make_unique<DataObj>(std::forward<T>(obj));
        // take a reference to the heap object, so that we can take its buffer
        // address later (we must use the heap object rather than the original
        // object because they may have different buffer addresses due to small
        // string optimization)
        auto& r = std::get<std::decay_t<T>>(*p);

        // create direct buffer based on the input object
        ::djinni::LocalRef<jobject> localData{env, env->NewDirectByteBuffer(r.data(), r.size())};
        ::djinni::jniExceptionCheck(env);
        _data = {env, localData.get()};
        _readonly = false;
        _len = r.size();
        _buf = reinterpret_cast<uint8_t*>(r.data());

        // register direct buffer with DataRefHelper.
        // p will be deleted by DataRefHelper.destroy()
        const auto& nativeObjectManagerClass = ::djinni::JniClass<NativeObjectManagerClassInfo>::get();
        env->CallStaticVoidMethod(nativeObjectManagerClass.classObject.get(),
                                  nativeObjectManagerClass.registerMethodId,
                                  localData.get(),
                                  ::djinni::JniClass<DataRefHelperClassInfo>::get().classObject.get(),
                                  reinterpret_cast<jlong>(p.get()));
        ::djinni::jniExceptionCheck(env);
        // NOLINTNEXTLINE(bugprone-unused-return-value)
        p.release(); // registration is successful, object is now managed by nativeObjectManager
    }
};

DataRef::DataRef(size_t len) {
    _impl = std::make_shared<DataRefJNI>(len);
}

// copy data into the new buffer object
DataRef::DataRef(const void* data, size_t len) {
    _impl = std::make_shared<DataRefJNI>(len);
    memcpy(mutableBuf(), data, len);
}

DataRef::DataRef(std::vector<uint8_t>&& vec) {
    _impl = std::make_shared<DataRefJNI>(std::move(vec));
}

DataRef::DataRef(std::string&& str) {
    _impl = std::make_shared<DataRefJNI>(std::move(str));
}

DataRef::DataRef(void* platformObj) {
    _impl = std::make_shared<DataRefJNI>(reinterpret_cast<jobject>(platformObj));
}

// NOLINTNEXTLINE
static void DataRefHelper_nativeDestroy(JNIEnv* /*unused*/, jclass /*unused*/, jlong nativeRef) {
    delete reinterpret_cast<DataRefJNI::DataObj*>(nativeRef);
}

static const JNINativeMethod kNativeMethods[] = {{
    const_cast<char*>("nativeDestroy"),
    const_cast<char*>("(J)V"),
    reinterpret_cast<void*>(&DataRefHelper_nativeDestroy),
}};

// NOLINTNEXTLINE
static auto sRegisterMethods =
    ::djinni::JNIMethodLoadAutoRegister("com/snapchat/djinni/DataRefHelper", kNativeMethods);

#elif DATAREF_OBJC

class DataRefObjc : public DataRef::Impl {
public:
    // create empty buffer from c++
    explicit DataRefObjc(size_t len) {
        allocate(len);
    }
    // create new data object and initialize with data. although this still
    // copies data, it does the allocation and initialization in one step.
    explicit DataRefObjc(const void* data, size_t len) {
        _mutableData = CFDataCreateMutable(kCFAllocatorDefault, len);
        assert(_mutableData != nullptr);
        CFDataAppendBytes(_mutableData, reinterpret_cast<const uint8_t*>(data), len);
        _data = _mutableData;
    }
    // take over a std::vector's buffer without copying it
    explicit DataRefObjc(std::vector<uint8_t>&& vec) {
        if (!vec.empty()) {
            takeOver(std::move(vec));
        } else {
            allocate(0);
        }
    }
    // take over a std::string's buffer without copying it
    explicit DataRefObjc(std::string&& str) {
        if (!str.empty()) {
            takeOver(std::move(str));
        } else {
            allocate(0);
        }
    }
    // wrap an immutable CFData object
    explicit DataRefObjc(CFDataRef data) {
        _data = data;
        _mutableData = nullptr;
        CFRetain(_data);
    }
    // wrap a mutable CFData object (CFMutableData)
    explicit DataRefObjc(CFMutableDataRef data) {
        _data = data;
        _mutableData = data;
        CFRetain(_data);
    }
    DataRefObjc(const DataRefObjc&) = delete;
    ~DataRefObjc() {
        CFRelease(_data);
    }

    const uint8_t* buf() const override {
        // must query from the CF object every time in case the buffer is
        // relocated.
        return CFDataGetBytePtr(_data);
    }
    size_t len() const override {
        // must query from the CF object every time in case the buffer is
        // resized.
        return CFDataGetLength(_data);
    }
    uint8_t* mutableBuf() override {
        return _mutableData ? CFDataGetMutableBytePtr(_mutableData) : nullptr;
    }

    PlatformObject platformObj() const override {
        return _data;
    }

private:
    CFDataRef _data;
    CFMutableDataRef _mutableData;

    void allocate(size_t len) {
        _mutableData = CFDataCreateMutable(kCFAllocatorDefault, len);
        assert(_mutableData != nullptr);
        CFDataSetLength(_mutableData, len);
        _data = _mutableData;
    }

    template<typename T>
    void takeOver(T&& obj) {
        using DataObj = std::decay_t<T>;
        CFAllocatorContext context = {};
        // create a new instance and steal the input object's buffer
        auto* p = new DataObj(std::forward<T>(obj));
        auto buf = reinterpret_cast<const uint8_t*>(p->data());
        auto len = p->size();
        context.info = p;
        context.deallocate = [](void* ptr, void* info) {
            // delete the wrapped object
            auto* p = reinterpret_cast<DataObj*>(info);
            delete p;
        };
        CFAllocatorRef deallocator = CFAllocatorCreate(kCFAllocatorDefault, &context);
        assert(deallocator != nullptr);
        // create the CFData without copying
        _data = CFDataCreateWithBytesNoCopy(nullptr, buf, len, deallocator);
        assert(_data != nullptr);
        CFRelease(deallocator); // CFDataCreateWithBytesNoCopy retains the object, so release here
        _mutableData = nullptr; // our CFData is immutable because it can't realloc and resize
    }
};

DataRef::DataRef(size_t len) {
    _impl = std::make_shared<DataRefObjc>(len);
}

DataRef::DataRef(const void* data, size_t len) {
    _impl = std::make_shared<DataRefObjc>(data, len);
}

DataRef::DataRef(std::vector<uint8_t>&& vec) {
    _impl = std::make_shared<DataRefObjc>(std::move(vec));
}
DataRef::DataRef(std::string&& str) {
    _impl = std::make_shared<DataRefObjc>(std::move(str));
}

DataRef::DataRef(CFMutableDataRef platformObj) {
    _impl = std::make_shared<DataRefObjc>(platformObj);
}

DataRef::DataRef(CFDataRef platformObj) {
    _impl = std::make_shared<DataRefObjc>(platformObj);
}

#elif DATAREF_WASM

class DataRefWasm : public DataRef::Impl {
public:
    // create empty buffer from c++
    explicit DataRefWasm(size_t len) {
        allocate(len);
    }
    // create new data object and initialize with data. although this still
    // copies data, it does the allocation and initialization in one step.
    explicit DataRefWasm(const void* data, size_t len) {
        auto* dbuf = new ::djinni::GenericBuffer<std::vector<uint8_t>>(
            reinterpret_cast<const uint8_t*>(data), len);
        // The js object will destroy the c++ object when its GCed
        _data = dbuf->createJsObject();
    }
    // take over a std::vector's buffer without copying it
    explicit DataRefWasm(std::vector<uint8_t>&& vec) {
        if (!vec.empty()) {
            auto* dbuf = new ::djinni::GenericBuffer<std::vector<uint8_t>>(std::move(vec));
            _data = dbuf->createJsObject();
        } else {
            allocate(0);
        }
    }
    // take over a std::string's buffer without copying it
    explicit DataRefWasm(std::string&& str) {
        if (!str.empty()) {
            auto* dbuf = new ::djinni::GenericBuffer<std::string>(std::move(str));
            _data = dbuf->createJsObject();
        } else {
            allocate(0);
        }
    }
    explicit DataRefWasm(PlatformObject data) {
        auto buffer = data["buffer"];
        assert(buffer == ::djinni::getWasmMemoryBuffer());
        _data = data;
    }
    DataRefWasm(const DataRefWasm&) = delete;

    const uint8_t* buf() const override {
        return reinterpret_cast<const uint8_t*>(_data["byteOffset"].as<unsigned>());
    }
    size_t len() const override {
        return static_cast<size_t>(_data["length"].as<unsigned>());
    }
    uint8_t* mutableBuf() override {
        return reinterpret_cast<uint8_t*>(_data["byteOffset"].as<unsigned>());
    }

    PlatformObject platformObj() const override {
        return _data;
    }

private:
    emscripten::val _data = emscripten::val::undefined();

    void allocate(size_t len) {
        auto* dbuf = new ::djinni::GenericBuffer<std::vector<uint8_t>>(len);
        _data = dbuf->createJsObject();
    }
};

DataRef::DataRef(size_t len) {
    _impl = std::make_shared<DataRefWasm>(len);
}

DataRef::DataRef(const void* data, size_t len) {
    _impl = std::make_shared<DataRefWasm>(data, len);
}

DataRef::DataRef(std::vector<uint8_t>&& vec) {
    _impl = std::make_shared<DataRefWasm>(std::move(vec));
}
DataRef::DataRef(std::string&& str) {
    _impl = std::make_shared<DataRefWasm>(std::move(str));
}

DataRef::DataRef(PlatformObject platformObj) {
    _impl = std::make_shared<DataRefWasm>(platformObj);
}

#else // Linux, pure c++

class DataRefCpp : public DataRef::Impl {
public:
    explicit DataRefCpp(size_t len) : _storage(len) {}
    DataRefCpp(const DataRefCpp&) = delete;
    explicit DataRefCpp(std::vector<uint8_t>&& vec) : _storage(std::move(vec)) {}

    const uint8_t* buf() const override {
        return _storage.data();
    }
    size_t len() const override {
        return _storage.size();
    }
    uint8_t* mutableBuf() override {
        return _storage.data();
    }

    PlatformObject platformObj() const override {
        return nullptr; // should not be called, as there's no translation
    }

private:
    std::vector<uint8_t> _storage;
};

DataRef::DataRef(size_t len) {
    _impl = std::make_shared<DataRefCpp>(len);
}

DataRef::DataRef(const void* data, size_t len) {
    _impl = std::make_shared<DataRefCpp>(len);
    memcpy(mutableBuf(), data, len);
}

DataRef::DataRef(std::vector<uint8_t>&& vec) {
    _impl = std::make_shared<DataRefCpp>(std::move(vec));
}

DataRef::DataRef(std::string&& str) : DataRef(str.data(), str.size()) {}

#endif

}} // namespace snapchat::djinni

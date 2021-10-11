/**
  * Copyright 2021 Snap, Inc.
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *    http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

#if DATAREF_JNI

#include "DataRef.hpp"
#include <cassert>

#include "djinni_support.hpp"
#include <variant>

namespace snapchat {
namespace djinni {

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

}} // namespace snapchat::djinni

#endif

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <cstring>

#if !defined(DATAREF_JNI) && !defined(DATAREF_OBJC)
  #if defined(__ANDROID__)
    #define DATAREF_JNI 1
  #elif defined(__APPLE__)
    #define DATAREF_OBJC 1
  #endif
#endif

#if DATAREF_JNI
  using PlatformObject = void*;
#elif DATAREF_OBJC
  #include <CoreFoundation/CFData.h>
  using PlatformObject = const void*;
#else
  using PlatformObject = const void*;
#endif

namespace snapchat {
namespace djinni {

class DataRef {
public:
    class Impl {
    public:
        virtual ~Impl() = default;
        virtual PlatformObject platformObj() const = 0;
        
        virtual const uint8_t* buf() const = 0;
        virtual size_t len() const = 0;
        virtual uint8_t* mutableBuf() = 0;
    };

    DataRef() = default;
    DataRef(const DataRef&) = default;
    DataRef(DataRef&&) = default;
    
    // initialize with empty buffer
    explicit DataRef(size_t len);
    // initialize with data
    DataRef(const void* data, size_t len);
    // initialize with copying vector
    explicit DataRef(const std::vector<uint8_t>& vec) : DataRef(vec.data(), vec.size()) {}
    // initialize with vector and try not to copy it
    explicit DataRef(std::vector<uint8_t>&& vec);
    // initialize with copying string
    explicit DataRef(const std::string& str) : DataRef(str.data(), str.size()) {}
    // initialize with string and try not to copy it
    explicit DataRef(std::string&& str);

#if DATAREF_JNI
    explicit DataRef(void* platformObj);
#elif DATAREF_OBJC
    DataRef(CFDataRef platformObj);
    DataRef(CFMutableDataRef platformObj);
#endif

    DataRef& operator=(const DataRef&) = default;
    DataRef& operator=(DataRef&&) = default;
    
    const uint8_t* buf() const {
        return _impl ? _impl->buf() : nullptr;
    }
    size_t len() const {
        return _impl ? _impl->len() : 0;
    }
    uint8_t* mutableBuf() const {
        return _impl ? _impl->mutableBuf() : nullptr;
    }
    PlatformObject platformObj() const {
        return _impl ? _impl->platformObj() : nullptr;
    }

private:
    std::shared_ptr<Impl> _impl;
};

}} // namespace snapchat::djinni

//
//  DJIMarshal+Private.h
//  Djinni
//
//  Created by knejp on 20.3.15.
//  Copyright (c) 2015 Dropbox. All rights reserved.
//

#pragma once
#import <Foundation/Foundation.h>
#import "DJOutcome.h"
#include <chrono>
#include <cstdint>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "../expected.hpp"
#import "DJFuture.h"
#import "Future.hpp"

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

namespace djinni {

struct Bool {
    using CppType = bool;
    using ObjcType = BOOL;

    static CppType toCpp(ObjcType x) noexcept { return x ? true : false; }
    static ObjcType fromCpp(CppType x) noexcept { return x ? YES : NO; }

    struct Boxed {
        using ObjcType = NSNumber*;
        static CppType toCpp(ObjcType x) noexcept { assert(x); return Bool::toCpp([x boolValue]); }
        static ObjcType fromCpp(CppType x) noexcept { return [NSNumber numberWithBool:Bool::fromCpp(x)]; }
    };
};

template<class Self, class T>
struct Primitive {
    using CppType = T;
    using ObjcType = T;

    static CppType toCpp(ObjcType x) noexcept { return x; }
    static ObjcType fromCpp(CppType x) noexcept { return x; }

    struct Boxed {
        using ObjcType = NSNumber*;
        static CppType toCpp(ObjcType x) noexcept { assert(x); return static_cast<CppType>(Self::unbox(x)); }
        static ObjcType fromCpp(CppType x) noexcept { return Self::box(x); }
    };
};

class I8 : public Primitive<I8, int8_t> {
    friend Primitive<I8, int8_t>;
    static char unbox(Boxed::ObjcType x) noexcept { return [x charValue]; }
    static Boxed::ObjcType box(CppType x) noexcept { return [NSNumber numberWithChar:static_cast<char>(x)]; }
};

class I16 : public Primitive<I16, int16_t> {
    friend Primitive<I16, int16_t>;
    static short unbox(Boxed::ObjcType x) noexcept { return [x shortValue]; }
    static Boxed::ObjcType box(CppType x) noexcept { return [NSNumber numberWithShort:static_cast<short>(x)]; }
};

class I32 : public Primitive<I32, int32_t> {
    friend Primitive<I32, int32_t>;
    static int unbox(Boxed::ObjcType x) noexcept { return [x intValue]; }
    static Boxed::ObjcType box(CppType x) noexcept { return [NSNumber numberWithInt:static_cast<int>(x)]; }
};

class I64 : public Primitive<I64, int64_t> {
    friend Primitive<I64, int64_t>;
    static long long unbox(Boxed::ObjcType x) noexcept { return [x longLongValue]; }
    static Boxed::ObjcType box(CppType x) noexcept { return [NSNumber numberWithLongLong:static_cast<long long>(x)]; }
};

class F32 : public Primitive<F32, float> {
    friend Primitive<F32, float>;
    static CppType unbox(Boxed::ObjcType x) noexcept { return [x floatValue]; }
    static Boxed::ObjcType box(CppType x) noexcept { return [NSNumber numberWithFloat:x]; }
};

class F64 : public Primitive<F64, double> {
    friend Primitive<F64, double>;
    static CppType unbox(Boxed::ObjcType x) noexcept { return [x doubleValue]; }
    static Boxed::ObjcType box(CppType x) noexcept { return [NSNumber numberWithDouble:x]; }
};

template<class CppEnum, class ObjcEnum>
struct Enum {
    using CppType = CppEnum;
    using ObjcType = ObjcEnum;

    static CppType toCpp(ObjcType e) noexcept { return static_cast<CppType>(e); }
    static ObjcType fromCpp(CppType e) noexcept { return static_cast<ObjcType>(e); }

    struct Boxed {
        using ObjcType = NSNumber*;
        static CppType toCpp(ObjcType x) noexcept { return toCpp(x, Tag<typename std::underlying_type<CppType>::type>()); }
        static ObjcType fromCpp(CppType x) noexcept { return fromCpp(x, Tag<typename std::underlying_type<CppType>::type>()); }

    private:
        template<class T> struct Tag { };

        static CppType toCpp(ObjcType x, Tag<int>) noexcept { return Enum::toCpp(static_cast<Enum::ObjcType>([x integerValue])); }
        static ObjcType fromCpp(CppType x, Tag<int>) noexcept { return [NSNumber numberWithInteger:static_cast<NSInteger>(Enum::fromCpp(x))]; }

        static CppType toCpp(ObjcType x, Tag<unsigned>) noexcept { return Enum::toCpp(static_cast<Enum::ObjcType>([x unsignedIntegerValue])); }
        static ObjcType fromCpp(CppType x, Tag<unsigned>) noexcept { return [NSNumber numberWithUnsignedInteger:static_cast<NSUInteger>(Enum::fromCpp(x))]; }
    };
};

struct String {
    using CppType = std::string;
    using ObjcType = NSString*;

    using Boxed = String;

    static CppType toCpp(ObjcType string) {
        assert(string);
        return {[string UTF8String], [string lengthOfBytesUsingEncoding:NSUTF8StringEncoding]};
    }

    static ObjcType fromCpp(const CppType& string) {
        assert(string.size() <= std::numeric_limits<NSUInteger>::max());
        return [[NSString alloc] initWithBytes:string.data()
                                        length:static_cast<NSUInteger>(string.size())
                                      encoding:NSUTF8StringEncoding];
    }
};

template<int wcharTypeSize>
static NSStringEncoding getWCharEncoding()
{
    static_assert(wcharTypeSize == 2 || wcharTypeSize == 4, "wchar_t must be represented by UTF-16 or UTF-32 encoding");
    return {}; // unreachable
}

template<>
inline NSStringEncoding getWCharEncoding<2>() {
    // case when wchar_t is represented by utf-16 encoding
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    return NSUTF16BigEndianStringEncoding;
#else
    return NSUTF16LittleEndianStringEncoding;
#endif
}

template<>
inline NSStringEncoding getWCharEncoding<4>() {
    // case when wchar_t is represented by utf-32 encoding
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    return NSUTF32BigEndianStringEncoding;
#else
    return NSUTF32LittleEndianStringEncoding;
#endif
}

struct WString {
    using CppType = std::wstring;
    using ObjcType = NSString*;

    using Boxed = WString;

    static CppType toCpp(ObjcType string) {
        assert(string);
        NSStringEncoding encoding = getWCharEncoding<sizeof(wchar_t)>();
        NSData* data = [string dataUsingEncoding:encoding];
        return std::wstring((wchar_t*)[data bytes], [data length] / sizeof (wchar_t));
    }

    static ObjcType fromCpp(const CppType& string) {
        assert(string.size() <= std::numeric_limits<NSUInteger>::max());
        return [[NSString alloc] initWithBytes:string.data()
                                     length:string.size() * sizeof(wchar_t)
                                     encoding:getWCharEncoding<sizeof(wchar_t)>()];
    }
};

struct Date {
    using CppType = std::chrono::system_clock::time_point;
    using ObjcType = NSDate*;

    using Boxed = Date;

    static CppType toCpp(ObjcType date) {
        using namespace std::chrono;
        static const auto POSIX_EPOCH = system_clock::from_time_t(0);
        auto timeIntervalSince1970 = duration<double>([date timeIntervalSince1970]);
        return POSIX_EPOCH + duration_cast<system_clock::duration>(timeIntervalSince1970);
    }

    static ObjcType fromCpp(const CppType& date) {
        using namespace std::chrono;
        static const auto POSIX_EPOCH = system_clock::from_time_t(0);
        return [NSDate dateWithTimeIntervalSince1970:duration_cast<duration<double>>(date - POSIX_EPOCH).count()];

    }
};

struct Binary {
    using CppType = std::vector<uint8_t>;
    using ObjcType = NSData*;

    using Boxed = Binary;

    static CppType toCpp(ObjcType data) {
        assert(data);
        auto bytes = reinterpret_cast<const uint8_t*>(data.bytes);
        return data.length > 0 ? CppType{bytes, bytes + data.length} : CppType{};
    }

    static ObjcType fromCpp(const CppType& bytes) {
        assert(bytes.size() <= std::numeric_limits<NSUInteger>::max());
        // Using the pointer from .data() on an empty vector is UB
        return bytes.empty() ? [NSData data] : [NSData dataWithBytes:bytes.data()
                                                              length:static_cast<NSUInteger>(bytes.size())];
    }
};

template<template<class> class OptionalType, class T>
class Optional {
    // SFINAE helper: if C::CppOptType exists, opt_type<T>(nullptr) will return
    // that type. If not, it returns OptionalType<C::CppType>. This is necessary
    // because we special-case optional interfaces to be represented as a nullable
    // std::shared_ptr<T>, not optional<shared_ptr<T>> or optional<nn<shared_ptr<T>>>.
    template <typename C> static OptionalType<typename C::CppType> opt_type(...);
    template <typename C> static typename C::CppOptType opt_type(typename C::CppOptType *);

public:
    using CppType = decltype(opt_type<T>(nullptr));
    using ObjcType = typename T::Boxed::ObjcType;

    using Boxed = Optional;

    static CppType toCpp(ObjcType obj) {
        if (obj) {
            return T::Boxed::toCpp(obj);
        } else {
            return CppType();
        }
    }

    // fromCpp used for normal optionals
    static ObjcType fromCpp(const OptionalType<typename T::CppType>& opt) {
        return opt ? T::Boxed::fromCpp(*opt) : nil;
    }

    // fromCpp used for nullable shared_ptr
    template <typename C = T>
    static ObjcType fromCpp(const typename C::CppOptType & cppOpt) {
        return T::Boxed::fromCppOpt(cppOpt);
    }
};

template<class T>
class List {
    using ECppType = typename T::CppType;
    using EObjcType = typename T::Boxed::ObjcType;

public:
    using CppType = std::vector<ECppType>;
    using ObjcType = NSArray*;

    using Boxed = List;

    static CppType toCpp(ObjcType array) {
        assert(array);
        auto v = CppType();
        v.reserve(array.count);
        for(EObjcType value in array) {
            v.push_back(T::Boxed::toCpp(value));
        }
        return v;
    }

    static ObjcType fromCpp(const CppType& v) {
        assert(v.size() <= std::numeric_limits<NSUInteger>::max());
        auto array = [NSMutableArray arrayWithCapacity:static_cast<NSUInteger>(v.size())];
        for(const auto& value : v) {
            [array addObject:T::Boxed::fromCpp(value)];
        }
        return [array copy];
    }
};

// ObjC does not have managed primitive array, so Array is the same as List in ObjC.
template<class T>
using Array = List<T>;

template<class T>
class Set {
    using ECppType = typename T::CppType;
    using EObjcType = typename T::Boxed::ObjcType;

public:
    using CppType = std::unordered_set<ECppType>;
    using ObjcType = NSSet*;

    using Boxed = Set;

    static CppType toCpp(ObjcType set) {
        assert(set);
        auto s = CppType();
        for(EObjcType value in set) {
            s.insert(T::Boxed::toCpp(value));
        }
        return s;
    }

    static ObjcType fromCpp(const CppType& s) {
        assert(s.size() <= std::numeric_limits<NSUInteger>::max());
        auto set = [NSMutableSet setWithCapacity:static_cast<NSUInteger>(s.size())];
        for(const auto& value : s) {
            [set addObject:T::Boxed::fromCpp(value)];
        }
        return [set copy];
    }
};

template<class Key, class Value>
class Map {
    using CppKeyType = typename Key::CppType;
    using CppValueType = typename Value::CppType;
    using ObjcKeyType = typename Key::Boxed::ObjcType;
    using ObjcValueType = typename Value::Boxed::ObjcType;

public:
    using CppType = std::unordered_map<CppKeyType, CppValueType>;
    using ObjcType = NSDictionary*;

    using Boxed = Map;

    static CppType toCpp(ObjcType map) {
        assert(map);
        __block auto m = CppType();
        m.reserve(map.count);
        [map enumerateKeysAndObjectsUsingBlock:^(ObjcKeyType key, ObjcValueType obj, BOOL *) {
            m.emplace(Key::Boxed::toCpp(key), Value::Boxed::toCpp(obj));
        }];
        return m;
    }

    static ObjcType fromCpp(const CppType& m) {
        assert(m.size() <= std::numeric_limits<NSUInteger>::max());
        auto map = [NSMutableDictionary dictionaryWithCapacity:static_cast<NSUInteger>(m.size())];
        for(const auto& kvp : m) {
            [map setObject:Value::Boxed::fromCpp(kvp.second) forKey:Key::Boxed::fromCpp(kvp.first)];
        }
        return [map copy];
    }
};

template<typename RESULT, typename ERROR>
class Outcome {
    using ResultCppType = typename RESULT::CppType;
    using ErrorCppType = typename ERROR::CppType;
    using ResultObjcType = typename RESULT::Boxed::ObjcType;
    using ErrorObjcType = typename ERROR::Boxed::ObjcType;
public:
    using CppType = expected<ResultCppType, ErrorCppType>;
    using ObjcType = DJOutcome*;

    using Boxed = Outcome;

    static CppType toCpp(ObjcType o) {
        assert(o);
        ResultObjcType r = [o result];
        if (r) {
            return RESULT::Boxed::toCpp(r);
        } else {
            ErrorObjcType e = [o error];
            return make_unexpected(ERROR::Boxed::toCpp(e));
        }
    }

    static ObjcType fromCpp(const CppType& c) {
        if (c.has_value()) {
            return [DJOutcome fromResult: RESULT::Boxed::fromCpp(c.value())];
        } else {
            return [DJOutcome fromError: ERROR::Boxed::fromCpp(c.error())];
        }
    }
};

template <typename CPP_PROTO, typename OBJC_PROTO>
class Protobuf {
public:
    using CppType = CPP_PROTO;
    using ObjcType = OBJC_PROTO*;

    using Boxed = Protobuf;

    static CppType toCpp(ObjcType o)
    {
        NSData* serialized = [o data];
        const void* bytes = [serialized bytes];
        int size = static_cast<int>([serialized length]);
        CPP_PROTO cppProto;

        [[maybe_unused]]
        bool success = cppProto.ParseFromArray(bytes, size);
        assert(success);

        return cppProto;
    }
    static ObjcType fromCpp(const CppType& c)
    {
        std::vector<uint8_t> buffer(c.ByteSizeLong());
        [[maybe_unused]]
        bool success = c.SerializeToArray(buffer.data(), static_cast<int>(buffer.size()));
        assert(success);
        NSData* serialized = [NSData dataWithBytesNoCopy:buffer.data() length:buffer.size() freeWhenDone:NO];
        NSError* error = nil;
        OBJC_PROTO* objcProto = [[OBJC_PROTO alloc] initWithData:serialized error:&error];
        return objcProto;
    }
};

template <typename CPP_PROTO>
class ProtobufPassthrough {
public:
    using CppType = CPP_PROTO;
    using ObjcType = CPP_PROTO;
    using Boxed = CPP_PROTO;

    static const CppType& toCpp(const ObjcType& o)
    {
        return o;
    }
    static const ObjcType& fromCpp(const CppType& c)
    {
        return c;
    }
};

template <class RESULT>
class FutureAdaptor
{
    using CppResType = typename RESULT::CppType;

public:
    using CppType = Future<CppResType>;
    using ObjcType = DJFuture*;

    using Boxed = FutureAdaptor;

    static CppType toCpp(ObjcType o)
    {
        using NativePromiseType = Promise<CppResType>;

        __block auto p = std::make_unique<NativePromiseType>();
        auto f = p->getFuture();
        [o then: ^id(id res) {p->setValue(RESULT::Boxed::toCpp(res)); return nil;}];
        return f;
    }

    static ObjcType fromCpp(CppType c)
    {
        DJPromise<typename RESULT::Boxed::ObjcType>* promise = [[DJPromise alloc] init];
        DJFuture<typename RESULT::Boxed::ObjcType>* future = [promise getFuture];

        c.then([promise] (Future<CppResType> res) {
                [promise setValue:RESULT::Boxed::fromCpp(res.get())];
            });
        
        return future;
    }
};

} // namespace djinni

#pragma once

#include "djinni_support.hpp"
#include "../cpp/Future.hpp"

namespace djinni::swift {

typedef void (*FutureCb)(void* ctx, AnyValue* result);
typedef void (*CleanupCb)(void* subscription);

struct AbstractFutureHolder: OpaqueValue {
    virtual ~AbstractFutureHolder() = default;
    virtual void setFutureCb(FutureCb cb, void* ctx) = 0;
};

template<typename RESULT>
struct FutureHolder: AbstractFutureHolder {
    using CppResType = typename RESULT::CppType;
    djinni::Future<CppResType> future;
    explicit FutureHolder(djinni::Future<CppResType> f) : future(std::move(f)) {}

    void setFutureCb(FutureCb cb, void* ctx) override {
        future.then([cb, ctx] (Future<CppResType> f) {
            try {
                auto v = RESULT::fromCpp(f.get());
                cb(ctx, &v);
            } catch (ErrorValue& e) {
                AnyValue errorValue = e;
                cb(ctx, &errorValue);
            }
        });
    }
};
template<>
struct FutureHolder<Void>: AbstractFutureHolder {
    djinni::Future<void> future;
    explicit FutureHolder(djinni::Future<void> f) : future(std::move(f)) {}

    void setFutureCb(FutureCb cb, void* ctx) override {
        future.then([cb, ctx] (Future<void> f) {
            try {
                auto v = makeVoidValue();
                cb(ctx, &v);
            } catch (ErrorValue& e) {
                AnyValue errorValue = e;
                cb(ctx, &errorValue);
            }
        });
    }
};

void setFutureCb(const AnyValue* futureValue, FutureCb cb, void* ctx);
AnyValue makeFutureValue(CleanupCb cleanup);
void setFutureResult(const AnyValue* futureValue, const AnyValue* futureResult);
void storeSubscription(const AnyValue* futureValue, void* subscription);

struct AnyValueFutureAdapter {
    using CppType = AnyValue;
    static AnyValue fromCpp(CppType c) {return c;}
};

template<>
struct FutureHolder<AnyValueFutureAdapter>: AbstractFutureHolder {
    djinni::Promise<AnyValue> promise;
    std::shared_ptr<djinni::Future<AnyValue>> future;
    void* subscription = nullptr;
    CleanupCb cleanup;
    FutureHolder(CleanupCb cleanup) {
        this->cleanup = cleanup;
        this->future = std::make_shared<djinni::Future<AnyValue>>(promise.getFuture());
    }
    ~FutureHolder() {
        cleanup(subscription);
    }
    void setFutureCb(FutureCb cb, void* ctx) override {}
    void setValue(const AnyValue* futureValue) {
        promise.setValue(*futureValue);
    }
};

template <class RESULT>
class FutureAdaptor
{
    using CppResType = typename RESULT::CppType;
public:
    using CppType = Future<CppResType>;

    static CppType toCpp(const AnyValue& o) {
        auto holder = std::dynamic_pointer_cast<FutureHolder<RESULT>>(std::get<OpaqueValuePtr>(o));
        if (holder) {
            return std::move(holder->future);
        }
        auto anyHolder = std::dynamic_pointer_cast<FutureHolder<AnyValueFutureAdapter>>(std::get<OpaqueValuePtr>(o));
        Promise<CppResType> p;
        auto f = p.getFuture();
        anyHolder->future->then([p = std::move(p)] (Future<AnyValue> f) mutable {
            auto res = f.get();
            if (std::holds_alternative<ErrorValue>(res)) {
                const auto& e = std::get<ErrorValue>(res);
                p.setException(e);
            } else {
                p.setValue(RESULT::toCpp(res));
            }
        });
        return f;
    }
    static AnyValue fromCpp(CppType c) {
        OpaqueValuePtr holder = std::make_shared<FutureHolder<RESULT>>(std::move(c));
        return {holder};
    }
};

template <>
class FutureAdaptor<Void>
{
public:
    using CppType = Future<void>;

    static CppType toCpp(const AnyValue& o) {
        auto holder = std::dynamic_pointer_cast<FutureHolder<Void>>(std::get<OpaqueValuePtr>(o));
        if (holder) {
            return std::move(holder->future);
        }
        auto anyHolder = std::dynamic_pointer_cast<FutureHolder<AnyValueFutureAdapter>>(std::get<OpaqueValuePtr>(o));
        Promise<void> p;
        auto f = p.getFuture();
        anyHolder->future->then([p = std::move(p)] (Future<AnyValue> f) mutable {
            auto res = f.get();
            if (std::holds_alternative<ErrorValue>(res)) {
                const auto& e = std::get<ErrorValue>(res);
                p.setException(e);
            } else {
                p.setValue();
            }
        });
        return f;
    }
    static AnyValue fromCpp(CppType c) {
        OpaqueValuePtr holder = std::make_shared<FutureHolder<Void>>(std::move(c));
        return {holder};
    }
};

}

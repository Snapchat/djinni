#include "Future_swift.hpp"

namespace djinni::swift {

void setFutureCb(const AnyValue* futureValue, FutureCb cb, void* ctx) {
    auto holder = std::dynamic_pointer_cast<AbstractFutureHolder>(std::get<OpaqueValuePtr>(*futureValue));
    holder->setFutureCb(cb, ctx);
}

AnyValue makeFutureValue(CleanupCb cleanup) {
    OpaqueValuePtr holder = std::make_shared<FutureHolder<AnyValueFutureAdapter>>(cleanup);
    return {holder};
}

void setFutureResult(const AnyValue* futureValue, const AnyValue* futureResult) {
    auto holder = std::dynamic_pointer_cast<FutureHolder<AnyValueFutureAdapter>>(std::get<OpaqueValuePtr>(*futureValue));
    holder->setValue(futureResult);
}

void storeSubscription(const AnyValue* futureValue, void* subscription) {
    auto holder = std::dynamic_pointer_cast<FutureHolder<AnyValueFutureAdapter>>(std::get<OpaqueValuePtr>(*futureValue));
    holder->subscription = subscription;
}

}

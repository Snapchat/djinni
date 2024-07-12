import DjinniSupportCxx
import Foundation
import Combine

// DJinni future<> maps to Combine.Future<> in Swift
public typealias DJFuture<T> = Future<T, DjinniError>

// Type erased interface for PromiseHolder because in futureCb() we don't have
// the type parameter.
protocol AbstractPromiseHolder: AnyObject {
    func fulfillPromise(value: UnsafePointer<djinni.swift.AnyValue>)
}
// The Swift Future wrapper object that can be fulfilled by a C++ call
class PromiseHolder<T: Marshaller>: AbstractPromiseHolder {
    var promise: DJFuture<T.SwiftType>.Promise
    init(marshaller: T.Type, promise: @escaping DJFuture<T.SwiftType>.Promise) {
        self.promise = promise
    }
    func fulfillPromise(value: UnsafePointer<djinni.swift.AnyValue>) {
        if (!djinni.swift.isError(value)) {
            promise(.success(T.fromCpp(value.pointee)))
        } else {
            promise(.failure(DjinniError(djinni.swift.getError(value))))
        }
    }
}
// A C++ friendly function. This is passed to C++ as the continuation routine of
// the C++ future. It calls the PromiseHolder and forwards the C++ future's
// result or error to the Swift future.
public func futureCb(
  ptr: UnsafeMutableRawPointer?,
  result: UnsafeMutablePointer<djinni.swift.AnyValue>?)
  -> Void {
    let ctx = Unmanaged<AnyObject>.fromOpaque(ptr!).takeRetainedValue()
    let promiseHolder = ctx as! AbstractPromiseHolder
    promiseHolder.fulfillPromise(value:result!)
}

// A C++ friendly function to release the subscription token stored with the C++
// future value.
public func cleanupCb(psubscription: UnsafeMutableRawPointer?) -> Void {
    let _ = Unmanaged<AnyCancellable>.fromOpaque(psubscription!).takeRetainedValue()
}

public enum FutureMarshaller<T: Marshaller>: Marshaller {
    public typealias SwiftType = DJFuture<T.SwiftType>
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        return Future() { promise in
            // Allocate the Swift future wrapper
            let promiseHolder = PromiseHolder(marshaller: T.self, promise: promise)
            let promiseHolderPtr = Unmanaged.passRetained(promiseHolder).toOpaque()
            // And connect it with the C++ future
            withUnsafePointer(to: v) { p in
                djinni.swift.setFutureCb(p, futureCb, promiseHolderPtr)
            }
        }
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        // Create a C++ future
        var futureValue = djinni.swift.makeFutureValue(cleanupCb)
        // Connect it with the Swift future
        let subscription = s.sink { completion in
            if case let .failure(e) = completion {
                var errorValue = djinni.swift.makeVoidValue()
                djinni.swift.setErrorValue(&errorValue, e.wrapped)
                djinni.swift.setFutureResult(&futureValue, &errorValue)
            }
        } receiveValue:{ v in
            var cppValue = T.toCpp(v)
            djinni.swift.setFutureResult(&futureValue, &cppValue)
        }
        // Store the subscription token so that the connection remains alive.
        let pSubscription = Unmanaged.passRetained(subscription).toOpaque()
        djinni.swift.storeSubscription(&futureValue, pSubscription)
        return futureValue
    }
}

import DjinniSupportCxx
import Foundation

public typealias Vtbl<T> = [(T, UnsafePointer<djinni.swift.ParameterList>?, UnsafeMutablePointer<djinni.swift.AnyValue>?) throws -> Void]

public class GenericProtocolWrapperContext {
    public func dispatch(idx: Int32, params: UnsafePointer<djinni.swift.ParameterList>?, ret: UnsafeMutablePointer<djinni.swift.AnyValue>?) -> Void {
        fatalError("pure virtual")
    }
    public func getInst() -> AnyObject {
        fatalError("pure virtual")
    }
}

public final class ProtocolWrapperContext<T>: GenericProtocolWrapperContext {
    var inst: T
    var vtbl: Vtbl<T>
    public init(inst: T, vtbl: Vtbl<T>) {
        self.inst = inst
        self.vtbl = vtbl
    }
    public override func dispatch(idx: Int32, params: UnsafePointer<djinni.swift.ParameterList>?, ret: UnsafeMutablePointer<djinni.swift.AnyValue>?) -> Void {
        do {
            try vtbl[Int(idx)](inst, params, ret)
        } catch let error as DjinniError {
            djinni.swift.setErrorValue(ret, error.wrapped)
        } catch {
            djinni.swift.setErrorMessage(ret, std.string(String(describing: error)))
        }
    }
    public override func getInst() -> AnyObject {
        return inst as AnyObject
    }
}

public func dispatcherProtocalCall(
  ptr: UnsafeMutableRawPointer?,
  idx: Int32,
  params: UnsafePointer<djinni.swift.ParameterList>?,
  ret: UnsafeMutablePointer<djinni.swift.AnyValue>?)
  -> Void {
    guard let pctx = ptr else { return }
    if (idx < 0) {
        // release and destroy the context
        let ctx = Unmanaged<GenericProtocolWrapperContext>.fromOpaque(pctx).takeRetainedValue()
        print("swift proxy cache before destroy", SwiftProxyCache.shared.mapPtrToProxy)
        let key = Unmanaged.passUnretained(ctx.getInst()).toOpaque()
        SwiftProxyCache.shared.mapPtrToProxy.removeValue(forKey: key)
        print("swift proxy cache after destroy", SwiftProxyCache.shared.mapPtrToProxy)
    } else {
        // dynamic dispatch on the vtbl
        let ctx = Unmanaged<GenericProtocolWrapperContext>.fromOpaque(pctx).takeUnretainedValue()
        ctx.dispatch(idx: idx, params: params, ret: ret)
    }
}

open class CppProxy {
    public var inst: djinni.swift.AnyValue
    public init(inst: djinni.swift.AnyValue) {
        self.inst = inst
    }
    deinit {
        destroyCppProxy(inst)
    }
}

class CppProxyCache {
    static let shared = CppProxyCache()
    // c++ objects in swift
    var mapPtrToProxy: [UnsafeRawPointer: UnsafeMutableRawPointer] = [:]
}

class SwiftProxyCache {
    static let shared = SwiftProxyCache()
    // swift objects in c++
    var mapPtrToProxy: [UnsafeMutableRawPointer: djinni.swift.WeakSwiftProxy] = [:]
}

// 1. object is a an existing cpp proxy  : return cpp proxy
// 2. object is a an existing swift proxy: unwrap the original swift object
// 3. need to create a new proxy         : call newProxyFunc
public func cppInterfaceToSwift<I>(_ c: djinni.swift.AnyValue,
                                   _ newProxyFunc: ()->I) -> I {
    return withUnsafePointer(to: c) { p in
        let info = djinni.swift.getInterfaceInfo(p)
        // for 1. check the cpp proxy cache
        if let s = CppProxyCache.shared.mapPtrToProxy[info.cppPointer] {
            return Unmanaged<AnyObject>.fromOpaque(s).takeUnretainedValue() as! I
        }
        // for 2. check if c++ ptr exists in SwiftProxyCache.mapCPtrToSwift
        if let pctx = info.ctxPointer {
            let ctx = Unmanaged<GenericProtocolWrapperContext>.fromOpaque(pctx).takeUnretainedValue()
            return ctx.getInst() as! I
        }
        // 3.
        let newProxy = newProxyFunc()
        CppProxyCache.shared.mapPtrToProxy[info.cppPointer] = Unmanaged.passUnretained(newProxy as AnyObject).toOpaque()
        return newProxy
    }
}

// 1. object is a an existing cpp proxy  : unwrap swift proxy
// 2. object is a an existing swift proxy: return existing
// 3. need to create a new proxy         : call newProxyFunc
public func swiftInterfaceToCpp<I>(_ s: I,
                                   _ newProxyFunc: ()->djinni.swift.AnyValue) -> djinni.swift.AnyValue {
    // 1. try cast to CppProxy and unwrap
    if let cppproxy = s as? CppProxy {
        return cppproxy.inst
    }
    // 2. check swift proxy cache
    let key = Unmanaged.passUnretained(s as AnyObject).toOpaque()
    if let weakProxy = SwiftProxyCache.shared.mapPtrToProxy[key] {
        return djinni.swift.strongify(weakProxy)
    }
    // 3.
    let newProxy = newProxyFunc()
    SwiftProxyCache.shared.mapPtrToProxy[key] = djinni.swift.weakify(newProxy)
    return newProxy
}

public func ctxPtr<I> (_ s: I, _ vtbl: Vtbl<I>) -> UnsafeMutableRawPointer {
    let ctx = ProtocolWrapperContext(inst: s, vtbl: vtbl)
    return Unmanaged.passRetained(ctx).toOpaque()
}

public func destroyCppProxy(_ inst: djinni.swift.AnyValue) {
    withUnsafePointer(to: inst) { p in
        let info = djinni.swift.getInterfaceInfo(p)
        print("before destroy cppproxycache entry", CppProxyCache.shared.mapPtrToProxy)
        CppProxyCache.shared.mapPtrToProxy.removeValue(forKey: info.cppPointer)
        print("after destroy cppproxycache entry", CppProxyCache.shared.mapPtrToProxy)
    }
}

public class DjinniError: Error {
    var wrapped: djinni.swift.ErrorValue
    init(_ wrapped: djinni.swift.ErrorValue) {
        self.wrapped = wrapped
    }
    public init(_ msg: String) {
        self.wrapped = djinni.swift.ErrorValue(std.string(msg))
    }
    public var errorMessage: String { return String(wrapped.msg) }
}

public func handleCppErrors(_ ret: UnsafePointer<djinni.swift.AnyValue>) throws {
    if (djinni.swift.isError(ret)) {
        throw DjinniError(djinni.swift.getError(ret))
    }
}

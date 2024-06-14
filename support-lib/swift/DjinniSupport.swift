import DjinniSupportCxx
import Foundation
import Combine

public typealias Vtbl<T> = [(T, UnsafePointer<djinni.swift.ParameterList>?, UnsafeMutablePointer<djinni.swift.AnyValue>?) -> Void]

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
        vtbl[Int(idx)](inst, params, ret)
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

public protocol Marshaller {
    associatedtype SwiftType
    static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType
    static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue
}

public enum EnumMarshaller<T: RawRepresentable>: Marshaller where T.RawValue == Int32 {
    public typealias SwiftType = T
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        return SwiftType(rawValue: djinni.swift.I32.toCpp(v))!
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return djinni.swift.I32.fromCpp(s.rawValue)
    }
}

public enum SmallIntMarshaller<T: BinaryInteger>: Marshaller {
    public typealias SwiftType = T
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        return SwiftType(exactly: djinni.swift.I32.toCpp(v))!
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return djinni.swift.I32.fromCpp(Int32(s))
    }
}
public enum I64Marshaller: Marshaller {
    public typealias SwiftType = Int64
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        return SwiftType(exactly: djinni.swift.I64.toCpp(v))!
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return djinni.swift.I64.fromCpp(s)
    }
}
public enum FloatMarshaller<T: BinaryFloatingPoint>: Marshaller {
    public typealias SwiftType = T
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        return T(djinni.swift.F64.toCpp(v))
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return djinni.swift.F64.fromCpp(Double(s))
    }
}
public enum BoolMarshaller: Marshaller {
    public typealias SwiftType = Bool
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        return djinni.swift.I32.toCpp(v) != 0
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return djinni.swift.I32.fromCpp(s ? 1 : 0)
    }
}

public typealias I8Marshaller = SmallIntMarshaller<Int8>
public typealias I16Marshaller = SmallIntMarshaller<Int16>
public typealias I32Marshaller = SmallIntMarshaller<Int32>
public typealias F32Marshaller = FloatMarshaller<Float>
public typealias F64Marshaller = FloatMarshaller<Double>

public enum StringMarshaller: Marshaller {
    public typealias SwiftType = String
    static public func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        let range = djinni.swift.getRange(v)
        let typedPtr = range.bytes.bindMemory(to: CChar.self, capacity: range.size)
        return String(cString: typedPtr)
    }
    static public func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        let bytes = s.utf8CString
        return bytes.withUnsafeBufferPointer { ptr in
            return djinni.swift.makeRangeValue(UnsafeMutableRawPointer(mutating: ptr.baseAddress!),
                                               ptr.count)
        }
    }
}

public enum BinaryMarshaller: Marshaller {
    public typealias SwiftType = Data
    static public func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        let range = djinni.swift.getRange(v)
        return Data(bytes: range.bytes, count: range.size)
    }
    static public func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return s.withUnsafeBytes { (ptr: UnsafeRawBufferPointer) in
            return djinni.swift.makeRangeValue(UnsafeMutableRawPointer(mutating: ptr.baseAddress!),
                                               ptr.count)
        }
    }
}

public enum DateMarshaller: Marshaller {
    public typealias SwiftType = Date
    static public func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        let millisecondsSinceEpoch = I64Marshaller.fromCpp(v)
        return Date(timeIntervalSince1970: Double(millisecondsSinceEpoch))
    }
    static public func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return I64Marshaller.toCpp(Int64(s.timeIntervalSince1970))
    }
}

public enum VoidMarshaller: Marshaller {
    public typealias SwiftType = Void
    static public func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        return ()
    }
    static public func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return djinni.swift.makeVoidValue()
    }
}

public enum OptionalMarshaller<T: Marshaller>: Marshaller {
    public typealias SwiftType = T.SwiftType?
    static public func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        return withUnsafePointer(to: v) { p in
            if (djinni.swift.isVoidValue(p)) {
                return nil
            }
            return T.fromCpp(v)
        }
    }
    static public func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        if (s == nil) {
            return djinni.swift.makeVoidValue()
        } else {
            return T.toCpp(s!)
        }
    }
}

public enum ListMarshaller<T: Marshaller>: Marshaller {
    public typealias SwiftType = [T.SwiftType]
    static public func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        var array = SwiftType()
        withUnsafePointer(to: v) { p in
            let size = djinni.swift.getSize(p)
            for i in 0..<size {
                array.append(T.fromCpp(djinni.swift.getMember(p, i)))
            }
        }
        return array
    }
    static public func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        var composite = djinni.swift.makeCompositeValue()
        for item in s {
            djinni.swift.addMember(&composite, T.toCpp(item))
        }
        return composite
    }
}

public typealias ArrayMarshaller<T: Marshaller> = ListMarshaller<T>

public enum SetMarshaller<T: Marshaller>: Marshaller where T.SwiftType: Hashable {
    public typealias SwiftType = Set<T.SwiftType>
    static public func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        var s = SwiftType()
        withUnsafePointer(to: v) { p in
            let size = djinni.swift.getSize(p)
            for i in 0..<size {
                s.insert(T.fromCpp(djinni.swift.getMember(p, i)))
            }
        }
        return s
    }
    static public func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        var composite = djinni.swift.makeCompositeValue()
        for item in s {
            djinni.swift.addMember(&composite, T.toCpp(item))
        }
        return composite
    }
}

public enum MapMarshaller<K: Marshaller, V: Marshaller>: Marshaller where K.SwiftType: Hashable {
    public typealias SwiftType = Dictionary<K.SwiftType, V.SwiftType>
    static public func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        var s = SwiftType()
        withUnsafePointer(to: v) { p in
            let size = djinni.swift.getSize(p)
            for i in stride(from: 0, to: size, by: 2) {
                s.updateValue(V.fromCpp(djinni.swift.getMember(p, i+1)),
                              forKey: K.fromCpp(djinni.swift.getMember(p, i)))
            }
        }
        return s
    }
    static public func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        var composite = djinni.swift.makeCompositeValue()
        for (k, v) in s {
            djinni.swift.addMember(&composite, K.toCpp(k))
            djinni.swift.addMember(&composite, V.toCpp(v))
        }
        return composite
    }
}

open class CppProxy {
    public var inst: djinni.swift.AnyValue
    public init(_ inst: djinni.swift.AnyValue) {
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

public typealias DJFuture<T> = Future<T, Never>

public enum FutureMarshaller<T: Marshaller>: Marshaller {
    public typealias SwiftType = DJFuture<T.SwiftType>
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        return Future() { promise in
            promise(Result.success(T.fromCpp(v)))
        }
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return djinni.swift.makeVoidValue()
    }
}

import support_lib_djinni_support_swiftxx

public class ProtocolWrapperContext {
    public typealias MethodDispatcher = (Any, UnsafePointer<djinni.swift.ParameterList>?, UnsafeMutablePointer<djinni.swift.AnyValue>?) -> Void

    var inst: Any
    var vtbl: [MethodDispatcher]

    public init(inst: Any, vtbl: [MethodDispatcher]) {
        self.inst = inst
        self.vtbl = vtbl
    }

    public func dispatch(idx: Int32, params: UnsafePointer<djinni.swift.ParameterList>?, ret: UnsafeMutablePointer<djinni.swift.AnyValue>?) -> Void {
        vtbl[Int(idx)](inst, params, ret)
    }
}

public func dispatcherProtocalCall(_  ptr: UnsafeMutableRawPointer?,
                                   idx: Int32,
                                   params: UnsafePointer<djinni.swift.ParameterList>?,
                                   ret: UnsafeMutablePointer<djinni.swift.AnyValue>?) -> Void {
    guard let pctx = ptr else { return }
    if (idx < 0) {
        // release and destroy the context
        let _ = Unmanaged<ProtocolWrapperContext>.fromOpaque(pctx).takeRetainedValue()
    } else {
        // dynamic dispatch on the vtbl
        let ctx = Unmanaged<ProtocolWrapperContext>.fromOpaque(pctx).takeUnretainedValue()
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

public enum StringMarshaller: Marshaller {
    public typealias SwiftType = String
    static public func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        return SwiftType(djinni.swift.String.toCpp(v))
    }
    static public func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return djinni.swift.String.fromCpp(std.string(s))
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

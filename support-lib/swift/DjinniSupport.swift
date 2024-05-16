import support_lib_djinni_support_common
import support_lib_djinni_support_swiftxx

public protocol Marshaller {
    associatedtype SwiftType
    static func fromCpp(_ v: djinni.AnyValue) -> SwiftType
    static func toCpp(_ s: SwiftType) -> djinni.AnyValue
}

public enum EnumMarshaller<T: RawRepresentable>: Marshaller where T.RawValue == Int32 {
    public typealias SwiftType = T
    public static func fromCpp(_ v: djinni.AnyValue) -> SwiftType {
        return SwiftType(rawValue: djinni.toI32(v))!
    }
    public static func toCpp(_ s: SwiftType) -> djinni.AnyValue {
        return djinni.fromI32(s.rawValue)
    }
}

public enum StringMarshaller: Marshaller {
    public typealias SwiftType = String
    static public func fromCpp(_ v: djinni.AnyValue) -> SwiftType {
        return String(djinni.toString(v))
    }
    static public func toCpp(_ s: SwiftType) -> djinni.AnyValue {
        return djinni.fromString(std.string(s))
    }
}

public enum ListMarshaller<T: Marshaller>: Marshaller {
    public typealias SwiftType = [T.SwiftType]
    static public func fromCpp(_ v: djinni.AnyValue) -> SwiftType {
        let size = djinni.getSize(v)
        var array = SwiftType()
        for i in 0..<size {
            array.append(T.fromCpp(djinni.getMember(v, i)))
        }
        return array
    }
    static public func toCpp(_ s: SwiftType) -> djinni.AnyValue {
        let composite = djinni.makeCompositeValue()
        for item in s {
            djinni.addMember(composite, T.toCpp(item))
        }
        return composite
    }
}

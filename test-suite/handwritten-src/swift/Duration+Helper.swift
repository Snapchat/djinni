import Foundation
import DjinniSupport
import DjinniSupportCxx

enum DurationS: Marshaller {
    typealias SwiftType = Duration
    static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        let seconds = F64Marshaller.fromCpp(v)
        return Duration.seconds(seconds)
    }
    static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        let v = s.components
        return F64Marshaller.toCpp(Double(v.seconds) + Double(v.attoseconds) * 1e-18)
    }
}
typealias DurationH = DurationS
typealias DurationMin = DurationS
typealias DurationMs = DurationS
typealias DurationUs = DurationS
typealias DurationNs = DurationS

public enum DurationMarshaller<Rep: Marshaller, Period: Marshaller>: Marshaller {
    public typealias SwiftType = Duration
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        return Period.fromCpp(v) as! Duration
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return Period.toCpp(s as! Period.SwiftType)
    }
}

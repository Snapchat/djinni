import DjinniSupportCxx
import Foundation

public enum OutcomeMarshaller<Res: Marshaller, Err: Marshaller>: Marshaller
  where Err.SwiftType: Error {
    public typealias SwiftType = Result<Res.SwiftType, Err.SwiftType>
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        withUnsafePointer(to: v) { p in
            var r = djinni.swift.getMember(p, 0)
            if (!djinni.swift.isVoidValue(&r)) {
                return .success(Res.fromCpp(r))
            } else {
                let e = djinni.swift.getMember(p, 1)
                return .failure(Err.fromCpp(e))
            }
        }
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        var comp = djinni.swift.makeCompositeValue()
        switch s {
        case .success(let r):
            djinni.swift.addMember(&comp, Res.toCpp(r))
        case .failure(let e):
            djinni.swift.addMember(&comp, djinni.swift.makeVoidValue())
            djinni.swift.addMember(&comp, Err.toCpp(e))
        }
        return comp
    }
}

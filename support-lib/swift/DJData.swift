import DjinniSupportCxx
import Foundation

public enum DataViewMarshaller: Marshaller {
    public typealias SwiftType = NSData
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        let range = djinni.swift.getBinaryRange(v)
        return SwiftType(bytesNoCopy: UnsafeMutableRawPointer(mutating: range.bytes!), length: range.size, deallocator:.none)
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return djinni.swift.makeRangeValue(s.bytes, s.length)
    }
}

public enum DataRefMarshaller: Marshaller {
    public typealias SwiftType = NSData
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        let range = djinni.swift.getBinaryRange(v)
        let cfdata = Unmanaged<CFData>.fromOpaque(range.bytes!).takeRetainedValue()
        return cfdata as NSData
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        if s is CFMutableData {
            let cfdataref = Unmanaged.passRetained(s as! CFMutableData).toOpaque()
            return djinni.swift.makeRangeValue(cfdataref, 1)
        } else {
            let cfdataref = Unmanaged.passRetained(s as CFData).toOpaque()
            return djinni.swift.makeRangeValue(cfdataref, 0)
        }
    }
}

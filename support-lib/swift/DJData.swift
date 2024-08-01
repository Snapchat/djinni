import DjinniSupportCxx
import Foundation

// DataView and DataRef are bridged to `NSData` instead of `Data` because `Data`
// uses small buffer optimization and it does not always provide a stable
// pointer that can be safely accessed in C++ (the pointer we can get from a
// small Data object will only work within the scope of `withUnsafeBytes` block)
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
// The C++ side implementation of DataRef uses CFData which is toll-free bridged
// to NSData.
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

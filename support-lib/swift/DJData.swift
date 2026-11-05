import DjinniSupportCxx
import Foundation

#if canImport(Darwin)
/// The Swift type of djinni's `binary` views and refs (DataView / DataRef): `NSData` on Apple platforms,
/// whose bytes stay put (see below) and whose CFData the C++ DataRef wraps. Elsewhere there's no
/// NSData (swift-foundation has no NS classes), so it's `Data`.
public typealias DJBinaryData = NSData

extension Data {
    /// This data as djinni's `binary` view / ref type.
    public var asDjinniData: DJBinaryData { self as NSData }
}

extension NSData {
    /// These bytes as `Data`.
    public var asData: Data { self as Data }
}

// DataView and DataRef are bridged to `NSData` instead of `Data` because `Data`
// uses small buffer optimization and it does not always provide a stable
// pointer that can be safely accessed in C++ (the pointer we can get from a
// small Data object will only work within the scope of `withUnsafeBytes` block)
public enum DataViewMarshaller: Marshaller {
    public typealias SwiftType = DJBinaryData
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        let range = djinni.swift.getBinaryRange(v)
        if (range.size == 0 || range.bytes == nil) {
            return SwiftType()
        }
        return SwiftType(bytesNoCopy: UnsafeMutableRawPointer(mutating: range.bytes!), length: range.size, deallocator:.none)
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return djinni.swift.makeRangeValue(s.bytes, s.length)
    }
}
// The C++ side implementation of DataRef uses CFData which is toll-free bridged
// to NSData.
public enum DataRefMarshaller: Marshaller {
    public typealias SwiftType = DJBinaryData
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        let range = djinni.swift.getBinaryRange(v)
        let cfdata = Unmanaged<CFData>.fromOpaque(range.bytes!).takeRetainedValue()
        return cfdata as NSData
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        if let nsMutableData = s as? NSMutableData {
            let cfDataRef = Unmanaged.passRetained(nsMutableData).toOpaque()
            return djinni.swift.makeRangeValue(cfDataRef, 1)
        } else {
            let cfDataRef = Unmanaged.passRetained(s as CFData).toOpaque()
            return djinni.swift.makeRangeValue(cfDataRef, 0)
        }
    }
}
#else
public typealias DJBinaryData = Data

extension Data {
    /// This data as djinni's `binary` view / ref type (itself, here).
    public var asDjinniData: DJBinaryData { self }
    /// These bytes as `Data` (itself, here).
    public var asData: Data { self }
}

// A DataView borrows: as with NSData on Apple, the bytes must outlive the call, so the Data must be
// one its owner keeps (and not small enough to be stored inline).
public enum DataViewMarshaller: Marshaller {
    public typealias SwiftType = DJBinaryData
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        let range = djinni.swift.getBinaryRange(v)
        if range.size == 0 || range.bytes == nil {
            return SwiftType()
        }
        return SwiftType(bytesNoCopy: UnsafeMutableRawPointer(mutating: range.bytes!), count: range.size, deallocator: .none)
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return s.withUnsafeBytes { djinni.swift.makeRangeValue($0.baseAddress, $0.count) }
    }
}

// Without CoreFoundation a DataRef crosses as a heap-held DataRef (see Data_swift.hpp): the C++ side
// hands one over that the Data wraps without copying, and Swift's bytes are copied into one going
// the other way.
public enum DataRefMarshaller: Marshaller {
    public typealias SwiftType = DJBinaryData
    public static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        let holder = djinni.swift.getBinaryRange(v).bytes!
        let length = djinni.swift.dataRefSize(holder)
        guard length > 0, let bytes = djinni.swift.dataRefBytes(holder) else {
            djinni.swift.releaseDataRef(holder)
            return SwiftType()
        }
        return SwiftType(
            bytesNoCopy: UnsafeMutableRawPointer(mutating: bytes),
            count: length,
            deallocator: .custom { _, _ in djinni.swift.releaseDataRef(holder) }
        )
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return s.withUnsafeBytes { djinni.swift.makeDataRefValue($0.baseAddress, $0.count) }
    }
}
#endif

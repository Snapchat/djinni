// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from multiple_inheritance.djinni

import DjinniSupport
import DjinniSupportCxx
import Foundation
import TestSuite
import TestSuiteCxx

/** Used for ObjC multiple inheritance tests */
public enum SecondListenerMarshaller: DjinniSupport.Marshaller {
    public typealias SwiftType = TestSuite.SecondListener
    public static func fromCpp(_ c: djinni.swift.AnyValue) -> SwiftType {
        return cppInterfaceToSwift(c, { fatalError("n/a") })
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return swiftInterfaceToCpp(s, { fatalError("n/a") })
    }
}

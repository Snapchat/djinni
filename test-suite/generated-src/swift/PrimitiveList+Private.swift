// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from primitive_list.djinni

import DjinniSupport
import DjinniSupportCxx
import Foundation
import TestSuite
import TestSuiteCxx

enum PrimitiveListMarshaller: DjinniSupport.Marshaller {
    typealias SwiftType = TestSuite.PrimitiveList
    static func fromCpp(_ c: djinni.swift.AnyValue) -> SwiftType {
        return withUnsafePointer(to: c) { p in
            let list = ListMarshaller<I64Marshaller>.fromCpp(djinni.swift.getMember(p, 0))
            return SwiftType(list: list)
        }
    }
    static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        var ret = djinni.swift.makeCompositeValue()
        djinni.swift.addMember(&ret, ListMarshaller<I64Marshaller>.toCpp(s.list))
        return ret
    }
}
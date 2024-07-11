// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from extended_record.djinni

import DjinniSupport
import DjinniSupportCxx
import Foundation
import TestSuite
import TestSuiteCxx

final class InterfaceUsingExtendedRecordCppProxy: DjinniSupport.CppProxy, TestSuite.InterfaceUsingExtendedRecord {
    init(_ inst: djinni.swift.AnyValue) { super.init(inst:inst) } 
    func meth(er: TestSuite.ExtendedRecord) throws -> TestSuite.ExtendedRecord {
        var params = djinni.swift.ParameterList()
        params.addValue(inst)
        params.addValue(ExtendedRecordMarshaller.toCpp(er))
        var ret = djinni_generated.InterfaceUsingExtendedRecord_meth(&params)
        try handleCppErrors(&ret)
        return ExtendedRecordMarshaller.fromCpp(ret)
    }
}
enum InterfaceUsingExtendedRecordMarshaller: DjinniSupport.Marshaller {
    typealias SwiftType = TestSuite.InterfaceUsingExtendedRecord
    static func fromCpp(_ c: djinni.swift.AnyValue) -> SwiftType {
        return cppInterfaceToSwift(c, { InterfaceUsingExtendedRecordCppProxy(c) as SwiftType })
    }
    static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return swiftInterfaceToCpp(s, { fatalError("n/a") })
    }
}
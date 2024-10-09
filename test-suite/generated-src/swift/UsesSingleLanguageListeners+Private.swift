// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from single_language_interfaces.djinni

import DjinniSupport
import DjinniSupportCxx
import Foundation
import TestSuite
import TestSuiteCxx

/**
 * Generating and compiling this makes sure other languages don't break
 * on references to interfaces they don't need.
 */
final class UsesSingleLanguageListenersCppProxy: DjinniSupport.CppProxy, TestSuite.UsesSingleLanguageListeners {
    init(_ inst: djinni.swift.AnyValue) { super.init(inst:inst) } 
    func callForObjC(_ l: TestSuite.ObjcOnlyListener) throws -> Void {
        var _params = djinni.swift.ParameterList()
        _params.addValue(inst)
        _params.addValue(ObjcOnlyListenerMarshaller.toCpp(l))
        var ret = djinni_generated.UsesSingleLanguageListeners_callForObjC(&_params)
        try handleCppErrors(&ret)
    }
    func returnForObjC() throws -> TestSuite.ObjcOnlyListener {
        var _params = djinni.swift.ParameterList()
        _params.addValue(inst)
        var ret = djinni_generated.UsesSingleLanguageListeners_returnForObjC(&_params)
        try handleCppErrors(&ret)
        return ObjcOnlyListenerMarshaller.fromCpp(ret)
    }
    func callForJava(_ l: TestSuite.JavaOnlyListener) throws -> Void {
        var _params = djinni.swift.ParameterList()
        _params.addValue(inst)
        _params.addValue(JavaOnlyListenerMarshaller.toCpp(l))
        var ret = djinni_generated.UsesSingleLanguageListeners_callForJava(&_params)
        try handleCppErrors(&ret)
    }
    func returnForJava() throws -> TestSuite.JavaOnlyListener {
        var _params = djinni.swift.ParameterList()
        _params.addValue(inst)
        var ret = djinni_generated.UsesSingleLanguageListeners_returnForJava(&_params)
        try handleCppErrors(&ret)
        return JavaOnlyListenerMarshaller.fromCpp(ret)
    }
}
public enum UsesSingleLanguageListenersMarshaller: DjinniSupport.Marshaller {
    public typealias SwiftType = TestSuite.UsesSingleLanguageListeners
    public static func fromCpp(_ c: djinni.swift.AnyValue) -> SwiftType {
        return cppInterfaceToSwift(c, { UsesSingleLanguageListenersCppProxy(c) as SwiftType })
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return swiftInterfaceToCpp(s, { fatalError("n/a") })
    }
}

// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from user_token.djinni

import DjinniSupport
import DjinniSupportCxx
import Foundation
import TestSuite
import TestSuiteCxx

final class UserTokenCppProxy: DjinniSupport.CppProxy, TestSuite.UserToken {
    init(_ inst: djinni.swift.AnyValue) { super.init(inst:inst) } 
    func whoami() throws -> String {
        var params = djinni.swift.ParameterList()
        params.addValue(inst)
        var ret = djinni_generated.UserToken_whoami(&params)
        try handleCppErrors(&ret)
        return StringMarshaller.fromCpp(ret)
    }
}
let userTokenMethods: Vtbl<UserToken> = [
    { inst, params, ret in
        djinni.swift.setReturnValue(ret, try StringMarshaller.toCpp(inst.whoami()))
    },
]

enum UserTokenMarshaller: DjinniSupport.Marshaller {
    typealias SwiftType = TestSuite.UserToken
    static func fromCpp(_ c: djinni.swift.AnyValue) -> SwiftType {
        return cppInterfaceToSwift(c, { UserTokenCppProxy(c) as SwiftType })
    }
    static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        return swiftInterfaceToCpp(s, { djinni_generated.UserTokenSwiftProxy.make(ctxPtr(s, userTokenMethods), dispatcherProtocalCall)})
    }
}
// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from primtypes.djinni

import DjinniSupport
import DjinniSupportCxx
import Foundation
import TestSuite
import TestSuiteCxx

public enum AssortedPrimitivesMarshaller: DjinniSupport.Marshaller {
    public typealias SwiftType = TestSuite.AssortedPrimitives
    public static func fromCpp(_ c: djinni.swift.AnyValue) -> SwiftType {
        return withUnsafePointer(to: c) { p in
            let b = BoolMarshaller.fromCpp(djinni.swift.getMember(p, 0))
            let eight = I8Marshaller.fromCpp(djinni.swift.getMember(p, 1))
            let sixteen = I16Marshaller.fromCpp(djinni.swift.getMember(p, 2))
            let thirtytwo = I32Marshaller.fromCpp(djinni.swift.getMember(p, 3))
            let sixtyfour = I64Marshaller.fromCpp(djinni.swift.getMember(p, 4))
            let fthirtytwo = F32Marshaller.fromCpp(djinni.swift.getMember(p, 5))
            let fsixtyfour = F64Marshaller.fromCpp(djinni.swift.getMember(p, 6))
            let oB = OptionalMarshaller<BoolMarshaller>.fromCpp(djinni.swift.getMember(p, 7))
            let oEight = OptionalMarshaller<I8Marshaller>.fromCpp(djinni.swift.getMember(p, 8))
            let oSixteen = OptionalMarshaller<I16Marshaller>.fromCpp(djinni.swift.getMember(p, 9))
            let oThirtytwo = OptionalMarshaller<I32Marshaller>.fromCpp(djinni.swift.getMember(p, 10))
            let oSixtyfour = OptionalMarshaller<I64Marshaller>.fromCpp(djinni.swift.getMember(p, 11))
            let oFthirtytwo = OptionalMarshaller<F32Marshaller>.fromCpp(djinni.swift.getMember(p, 12))
            let oFsixtyfour = OptionalMarshaller<F64Marshaller>.fromCpp(djinni.swift.getMember(p, 13))
            return SwiftType(b: b, eight: eight, sixteen: sixteen, thirtytwo: thirtytwo, sixtyfour: sixtyfour, fthirtytwo: fthirtytwo, fsixtyfour: fsixtyfour, oB: oB, oEight: oEight, oSixteen: oSixteen, oThirtytwo: oThirtytwo, oSixtyfour: oSixtyfour, oFthirtytwo: oFthirtytwo, oFsixtyfour: oFsixtyfour)
        }
    }
    public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        var ret = djinni.swift.makeCompositeValue()
        djinni.swift.addMember(&ret, BoolMarshaller.toCpp(s.b))
        djinni.swift.addMember(&ret, I8Marshaller.toCpp(s.eight))
        djinni.swift.addMember(&ret, I16Marshaller.toCpp(s.sixteen))
        djinni.swift.addMember(&ret, I32Marshaller.toCpp(s.thirtytwo))
        djinni.swift.addMember(&ret, I64Marshaller.toCpp(s.sixtyfour))
        djinni.swift.addMember(&ret, F32Marshaller.toCpp(s.fthirtytwo))
        djinni.swift.addMember(&ret, F64Marshaller.toCpp(s.fsixtyfour))
        djinni.swift.addMember(&ret, OptionalMarshaller<BoolMarshaller>.toCpp(s.oB))
        djinni.swift.addMember(&ret, OptionalMarshaller<I8Marshaller>.toCpp(s.oEight))
        djinni.swift.addMember(&ret, OptionalMarshaller<I16Marshaller>.toCpp(s.oSixteen))
        djinni.swift.addMember(&ret, OptionalMarshaller<I32Marshaller>.toCpp(s.oThirtytwo))
        djinni.swift.addMember(&ret, OptionalMarshaller<I64Marshaller>.toCpp(s.oSixtyfour))
        djinni.swift.addMember(&ret, OptionalMarshaller<F32Marshaller>.toCpp(s.oFthirtytwo))
        djinni.swift.addMember(&ret, OptionalMarshaller<F64Marshaller>.toCpp(s.oFsixtyfour))
        return ret
    }
}

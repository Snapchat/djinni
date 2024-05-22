import TextSort                 // Swift Djinni API
import DjinniSupport            // Swift support library
import support_lib_djinni_support_swiftxx // C++ support library

enum ItemListMarshaller: Marshaller {
    typealias SwiftType = ItemList
    static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        return withUnsafePointer(to: v) { p in
            // generate one line for each member of the record
            let items = DjinniSupport.ListMarshaller<DjinniSupport.StringMarshaller>.fromCpp(djinni.swift.getMember(p, 0))
            // generate a struct constructor call
            return ItemList(items: items)
        }
    }
    static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue {
        // construct the record container
        var v = djinni.swift.makeCompositeValue()
        // generate one line for each member of the record
        djinni.swift.addMember(&v, ListMarshaller<DjinniSupport.StringMarshaller>.toCpp(s.items))
        return v
    }
}

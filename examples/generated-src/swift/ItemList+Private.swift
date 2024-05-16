import TextSort                 // Swift Djinni API
import DjinniSupport            // Swift support library
import support_lib_djinni_support_swiftxx // C++ support library

enum ItemListMarshaller: Marshaller {
    typealias SwiftType = ItemList
    static func fromCpp(_ v: djinni.AnyValue) -> SwiftType {
        let items = DjinniSupport.ListMarshaller<DjinniSupport.StringMarshaller>.fromCpp(djinni.getMember(v, 0))
        return ItemList(items: items)
    }
    static func toCpp(_ s: SwiftType) -> djinni.AnyValue {
        let v = djinni.makeCompositeValue()
        djinni.addMember(v, ListMarshaller<DjinniSupport.StringMarshaller>.toCpp(s.items))
        return v
    }
}

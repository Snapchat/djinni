import TextSort                 // Swift Djinni API
import DjinniSupport            // Swift support library
import support_lib_djinni_support_swiftxx // C++ support library
import examples_textsort_swiftxx          // C++ bridge code

class SortItemsProxy: SortItems {
    var inst: djinni.AnyValue
    init(_ inst: djinni.AnyValue) {
        self.inst = inst
    }
    func sort(order: SortOrder, items: ItemList) {
        var params = djinni.ParameterList()
        params.addValue(inst)
        params.addValue(SortOrderMarshaller.toCpp(order))
        params.addValue(ItemListMarshaller.toCpp(items))
        djinni_generated.SortItems_sort(params)
    }
}

enum SortItemsMarshaller: DjinniSupport.Marshaller {
    typealias SwiftType = SortItems
    static func fromCpp(_ v: djinni.AnyValue) -> SwiftType {
        return SortItemsProxy(v)
    }
    static func toCpp(_ v: SwiftType) -> djinni.AnyValue {
        return djinni.nilValue() // no +swift
    }
}

public class SortItems_statics {
    static func createWithListener(listener: TextboxListener)-> SortItems {
        var params = djinni.ParameterList()
        params.addValue(TextboxListenerMarshaller.toCpp(listener))
        let ret = djinni_generated.SortItems_createWithListener(params)
        return SortItemsMarshaller.fromCpp(ret)
    }
    static func runSort(items: ItemList)-> ItemList {
        var params = djinni.ParameterList()
        params.addValue(ItemListMarshaller.toCpp(items))
        let ret = djinni_generated.SortItems_runSort(params)
        return ItemListMarshaller.fromCpp(ret)
    }
}

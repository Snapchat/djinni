import TextSort                 // Swift Djinni API
import DjinniSupport            // Swift support library
import support_lib_djinni_support_swiftxx // C++ support library
import examples_textsort_swiftxx          // C++ bridge code

// generate Proxy class if interface is +c
class SortItemsProxy: SortItems {
    // generate fixed member and init
    var inst: djinni.AnyValue
    init(_ inst: djinni.AnyValue) {
        self.inst = inst
    }
    // generate method stubs
    func sort(order: SortOrder, items: ItemList) {
        var params = djinni.ParameterList()
        // add instance as first parameter
        params.addValue(inst)
        // generate a line for each parameter
        params.addValue(SortOrderMarshaller.toCpp(order))
        params.addValue(ItemListMarshaller.toCpp(items))
        // forward to C++ implementation
        djinni_generated.SortItems_sort(&params)
        // convert result back to swift (if return type is not void)
    }
}

enum SortItemsMarshaller: DjinniSupport.Marshaller {
    typealias SwiftType = SortItems
    // generate fromCpp() if interface is +c
    static func fromCpp(_ v: djinni.AnyValue) -> SwiftType {
        return SortItemsProxy(v)
    }
    // generate unimplemented toCpp() if interface lacks +swift
    static func toCpp(_ v: SwiftType) -> djinni.AnyValue {
        fatalError("n/a")
    }
}

// Generate stubs for static methods (if there are any)
public class SortItems_statics {
    static func createWithListener(listener: TextboxListener)-> SortItems {
        // build prameter list
        var params = djinni.ParameterList()
        // generate one line for each parameter
        params.addValue(TextboxListenerMarshaller.toCpp(listener))
        // forward to C++ implementation
        let ret = djinni_generated.SortItems_createWithListener(&params)
        // convert result back to swift
        return SortItemsMarshaller.fromCpp(ret)
    }
    static func runSort(items: ItemList)-> ItemList {
        var params = djinni.ParameterList()
        params.addValue(ItemListMarshaller.toCpp(items))
        let ret = djinni_generated.SortItems_runSort(&params)
        return ItemListMarshaller.fromCpp(ret)
    }
}

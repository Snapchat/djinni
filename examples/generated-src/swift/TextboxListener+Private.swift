import examples_textsort_swiftxx
import TextSort
import DjinniSupport
import support_lib_djinni_support_swiftxx

typealias Vtbl<T> = [(T, UnsafePointer<djinni.swift.ParameterList>?, UnsafeMutablePointer<djinni.swift.AnyValue>?) -> Void]

let textboxListenerMethods: Vtbl<TextboxListener> = [
  // generate one entry for each interface method
  { inst, params, ret in
      // generate one line for each parameter 
      let items = ItemListMarshaller.fromCpp(djinni.swift.getMember(params, 0))
      // call swift object through protocol
      inst.update(items: items)
      // update ret if method has return value
  },
]

enum TextboxListenerMarshaller: DjinniSupport.Marshaller {
    typealias SwiftType = TextboxListener
    static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        return cppInterfaceToSwift(v, {_ in fatalError("n/a")})
    }
    // generate toCpp() with the dispatch func
    static func toCpp(_ listener: SwiftType) -> djinni.swift.AnyValue {
        return swiftInterfaceToCpp(listener,
                                   {s in
                                       let ctx = ProtocolWrapperContext(inst: s, vtbl: textboxListenerMethods)
                                       return djinni_generated.TextboxListenerSwiftProxy.make(Unmanaged.passRetained(ctx).toOpaque(), DjinniSupport.dispatcherProtocalCall)})
    }
}

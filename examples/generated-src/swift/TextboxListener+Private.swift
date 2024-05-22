import examples_textsort_swiftxx
import TextSort
import DjinniSupport
import support_lib_djinni_support_swiftxx

// have to declare this type in each file because for some reason we can't export typealiases with C++ types from DjinniSupport.swift
typealias MethodDispatcher = (Any, UnsafePointer<djinni.swift.ParameterList>?, UnsafeMutablePointer<djinni.swift.AnyValue>?) -> Void

let textboxListenerMethods: [MethodDispatcher] = [
  // generate one entry for each interface method
  { inst, params, ret in
      // generate one line for each parameter 
      let items = ItemListMarshaller.fromCpp(djinni.swift.getMember(params, 0))
      // call swift object through protocol
      (inst as! TextboxListener).update(items: items)
      // update ret if method has return value
  },
]

enum TextboxListenerMarshaller: DjinniSupport.Marshaller {
    typealias SwiftType = TextboxListener
    // generate unimplemented fromCpp() if interface lacks +c
    static func fromCpp(_ v: djinni.swift.AnyValue) -> SwiftType {
        fatalError("n/a")
    }
    // generate toCpp() with the dispatch func
    static func toCpp(_ listener: SwiftType) -> djinni.swift.AnyValue {
        let ctx = ProtocolWrapperContext(inst: listener, vtbl: textboxListenerMethods)
        let p = Unmanaged.passRetained(ctx).toOpaque()
        return djinni_generated.TextboxListenerProtocolWrapper.make(p, DjinniSupport.dispatcherProtocalCall)
    }
}

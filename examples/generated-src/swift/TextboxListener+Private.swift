import examples_textsort_swiftxx
import TextSort
import DjinniSupport
import support_lib_djinni_support_swiftxx

class TextboxListenerProxy: TextboxListener {
    var inst: djinni.AnyValue
    init(_ inst: djinni.AnyValue) {
        self.inst = inst
    }
    func update(items: ItemList) {
        var params = djinni.ParameterList()
        params.addValue(inst)
        params.addValue(ItemListMarshaller.toCpp(items))
        djinni_generated.TextboxListener_update(&params)
    }
}

func textboxListenerDispatcher(_  instance: UnsafeMutableRawPointer?,
                               idx: Int32,
                               params: UnsafePointer<djinni.ParameterList>?,
                               ret: UnsafeMutablePointer<djinni.AnyValue>?) -> Void {
    guard let ptr = instance else { return }
    if (idx < 0) {
        let _ = Unmanaged<AnyObject>.fromOpaque(ptr).takeRetainedValue()
        return
    }
    guard let listener = Unmanaged<AnyObject>.fromOpaque(ptr).takeUnretainedValue() as? TextboxListener else { return }
    if (idx == 0) {
        let items = ItemListMarshaller.fromCpp(djinni.getMember(params, 0))
        listener.update(items: items)
    }
}

enum TextboxListenerMarshaller: DjinniSupport.Marshaller {
    typealias SwiftType = TextboxListener
    static func fromCpp(_ v: djinni.AnyValue) -> SwiftType {
        return TextboxListenerProxy(v)
    }
    static func toCpp(_ listener: TextboxListener) -> djinni.AnyValue {
        let instance = Unmanaged.passRetained(listener as AnyObject).toOpaque()
        return djinni_generated.TextboxListenerProtocolWrapper.make(instance, textboxListenerDispatcher)
    }
}

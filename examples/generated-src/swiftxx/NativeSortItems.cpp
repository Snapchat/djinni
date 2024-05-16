#include <NativeSortItems.hpp>
#include <item_list.hpp>

namespace djinni_generated {

struct NativeItemList {
    static djinni::AnyValue fromCpp(const textsort::ItemList& v) {
        auto ret = std::make_shared<djinni::CompositeValue>();
        ret->addValue(djinni::List<djinni::String>::fromCpp(v.items));
        return {ret};
    }
    static textsort::ItemList toCpp(const djinni::AnyValue& v) {
        auto c = std::get<djinni::CompositeValuePtr>(v);
        return textsort::ItemList(djinni::List<djinni::String>::toCpp(c->getValue(0)));
    }
};


void TextboxListenerProtocolWrapper::update(const textsort::ItemList& items) {
    djinni::ParameterList params;
    params.addValue(NativeItemList::fromCpp(items));
    callProtocol(0, params);
}


struct NativeTextboxListener {
    static std::shared_ptr<textsort::TextboxListener> toCpp(const djinni::AnyValue& v) {
        auto p = std::get<djinni::InterfaceValue>(v);
        return std::reinterpret_pointer_cast<textsort::TextboxListener>(p);
    }
};

struct NativeSortItems {
    static djinni::AnyValue fromCpp(const std::shared_ptr<textsort::SortItems>& v) {
        return {v};
    }
    static std::shared_ptr<textsort::SortItems> toCpp(const djinni::AnyValue& v) {
        auto p = std::get<djinni::InterfaceValue>(v);
        return std::reinterpret_pointer_cast<textsort::SortItems>(p);
    }
};

struct NativeSortOrder {
    static djinni::AnyValue fromCpp(textsort::sort_order v) {
        return {static_cast<int32_t>(v)};
    }
    static textsort::sort_order toCpp(const djinni::AnyValue& v) {
        auto i = std::get<djinni::I32Value>(v);
        return static_cast<textsort::sort_order>(i);
    }
};

djinni::AnyValue SortItems_createWithListener(const djinni::ParameterList& params) {
    auto listener = NativeTextboxListener::toCpp(params.getValue(0));
    auto ret = textsort::SortItems::create_with_listener(listener);
    return NativeSortItems::fromCpp(ret);
}

djinni::AnyValue SortItems_runSort(const djinni::ParameterList& params) {
    auto items = NativeItemList::toCpp(params.getValue(0));
    auto ret = textsort::SortItems::run_sort(items);
    return NativeItemList::fromCpp(ret);
}

djinni::AnyValue SortItems_sort(const djinni::ParameterList& params) {
    auto inst = NativeSortItems::toCpp(params.getValue(0));
    auto order = NativeSortOrder::toCpp(params.getValue(1));
    auto items = NativeItemList::toCpp(params.getValue(2));
    inst->sort(order, items);
    return djinni::nilValue();
}

djinni::AnyValue TextboxListener_update(const djinni::ParameterList& params) {
    auto inst = NativeTextboxListener::toCpp(params.getValue(0));
    auto items = NativeItemList::toCpp(params.getValue(1));
    inst->update(items);
    return djinni::nilValue();
}

djinni::AnyValue TextboxListenerProtocolWrapper::make(void* instance, djinni::DispatchFunc dispatcher) {
    return {std::make_shared<TextboxListenerProtocolWrapper>(instance, dispatcher)};
}

}

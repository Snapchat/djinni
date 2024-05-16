#include <NativeSortItems.hpp>
#include <NativeTextboxListener.hpp>
#include <NativeItemList.hpp>
#include <NativeSortOrder.hpp>

namespace djinni_generated {

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

}

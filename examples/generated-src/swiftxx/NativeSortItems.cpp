#include <NativeSortItems.hpp>
#include <NativeTextboxListener.hpp>
#include <NativeItemList.hpp>
#include <NativeSortOrder.hpp>

namespace djinni_generated {

djinni::swift::AnyValue SortItems_createWithListener(const djinni::swift::ParameterList* params) {
    // static method:
    // generate a line for each parameter of the method
    auto listener = NativeTextboxListener::toCpp(params->getValue(0));
    // call the C++ static method implementation
    auto ret = textsort::SortItems::create_with_listener(listener);
    // convert result to intermediate form
    return NativeSortItems::fromCpp(ret);
}

djinni::swift::AnyValue SortItems_runSort(const djinni::swift::ParameterList* params) {
    // static method:
    auto items = NativeItemList::toCpp(params->getValue(0));
    auto ret = textsort::SortItems::run_sort(items);
    return NativeItemList::fromCpp(ret);
}

// +c instance method stub
djinni::swift::AnyValue SortItems_sort(const djinni::swift::ParameterList* params) {
    // instance method:
    // retrieve the instance pointer from params[0]
    auto inst = NativeSortItems::toCpp(params->getValue(0));
    // generate a line for each parameter of the method
    auto order = NativeSortOrder::toCpp(params->getValue(1));
    auto items = NativeItemList::toCpp(params->getValue(2));
    // call the C++ instance method implementation
    inst->sort(order, items);
    // convert result to intermediate form
    // return nil for void methods
    return djinni::swift::makeVoidValue();
}

}

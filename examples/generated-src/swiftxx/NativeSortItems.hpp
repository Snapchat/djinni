#pragma once

#include "sort_items.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

// Alias to generic interface marshaller 
using NativeSortItems = djinni::swift::Interface<textsort::SortItems>;

// Generate forward declarations for each static and +c instance method
djinni::swift::AnyValue SortItems_createWithListener(const djinni::swift::ParameterList* params);
djinni::swift::AnyValue SortItems_runSort(const djinni::swift::ParameterList* params);
djinni::swift::AnyValue SortItems_sort(const djinni::swift::ParameterList* params);

}

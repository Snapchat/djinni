#pragma once

#include "sort_items.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

// Alias to generic interface marshaller 
using NativeSortItems = djinni::Interface<textsort::SortItems>;

// Generate forward declarations for each static and instance method
djinni::AnyValue SortItems_createWithListener(const djinni::ParameterList* params);
djinni::AnyValue SortItems_runSort(const djinni::ParameterList* params);
djinni::AnyValue SortItems_sort(const djinni::ParameterList* params);

}

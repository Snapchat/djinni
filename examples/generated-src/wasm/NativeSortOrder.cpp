// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from example.djinni

#include "NativeSortOrder.hpp"  // my header

namespace djinni_generated {

namespace textsort {
    EM_JS(void, djinni_init_sort_order, (), {
        Module.SortOrder =  {
            ASCENDING : 0,
            DESCENDING : 1,
            RANDOM : 2,
        }
    })
}

EMSCRIPTEN_BINDINGS(sort_order) {
    textsort::djinni_init_sort_order();
}

}  // namespace djinni_generated
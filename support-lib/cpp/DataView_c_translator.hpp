#pragma once

#include "djinni_c.h"
#include "DataView.hpp"

namespace djinni::c_api {

class DataViewTranslator {
public:
  static ::djinni::DataView toCpp(djinni_binary_ref binary);
  static djinni_binary_ref fromCpp(const ::djinni::DataView &dataRef);
};


}
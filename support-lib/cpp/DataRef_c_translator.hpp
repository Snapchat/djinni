#pragma once

#include "djinni_c.h"
#include "DataRef.hpp"

namespace djinni::c_api {

class DataRefTranslator {
public:
  static ::djinni::DataRef toCpp(djinni_binary_ref binary);
  static djinni_binary_ref fromCpp(const ::djinni::DataRef &dataRef);
};

}
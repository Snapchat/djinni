#pragma once

#include "ObjectNative.hpp"

namespace snapchat::djinni::benchmark {

class ObjectNativeImpl : public ObjectNative {
public:
    void baseline() override;
};

} // namespace snap::djinni_perf_benchmark

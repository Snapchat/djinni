#pragma once
#include "djinni_wasm.hpp"
#include <cassert>
#include <chrono>

namespace djinni
{
    template<class Ratio>
    struct DurationPeriod;

    using Duration_h   = DurationPeriod<std::ratio<3600>>;
    using Duration_min = DurationPeriod<std::ratio<60>>;
    using Duration_s   = DurationPeriod<std::ratio<1>>;
    using Duration_ms  = DurationPeriod<std::milli>;
    using Duration_us  = DurationPeriod<std::micro>;
    using Duration_ns  = DurationPeriod<std::nano>;

    template<class Rep, class Period>
    struct Duration;

    template<class Rep, class Ratio>
    struct Duration<Rep, DurationPeriod<Ratio>>
    {
        using CppType = std::chrono::duration<typename Rep::CppType, Ratio>;
        using JsType = em::val; // js duration is a plain number (milliseconds in double)

        using Boxed = Duration;

        static CppType toCpp(const JsType& j)
        {
            return std::chrono::duration_cast<CppType>(std::chrono::nanoseconds(static_cast<int64_t>(j.as<double>() * 1000000)));
        }
        static JsType fromCpp(const CppType& c)
        {
            return JsType{static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(c).count()) / 1000000};
        }
    };
}

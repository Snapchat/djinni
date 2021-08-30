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
        using JsType = em::val; // js duration is a plain number (milliseconds)

        using Boxed = Duration;

        static CppType toCpp(const JsType& j)
        {
            return std::chrono::duration_cast<CppType>(std::chrono::milliseconds(static_cast<int64_t>(j.as<double>())));
        }
        static JsType fromCpp(const CppType& c)
        {
            return JsType{std::chrono::duration_cast<std::chrono::milliseconds>(c).count()};
        }
    };
}

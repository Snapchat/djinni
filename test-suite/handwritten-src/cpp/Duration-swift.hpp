#pragma once
#include "djinni_support.hpp"
#include <chrono>

namespace djinni::swift
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

        using Boxed = Duration;

        static CppType toCpp(const AnyValue& s)
        {
            return std::chrono::duration_cast<CppType>(std::chrono::duration<double>{std::get<DoubleValue>(s)});
        }
        static AnyValue fromCpp(const CppType& c)
        {
            return { std::chrono::duration_cast<std::chrono::duration<double>>(c).count() };
        }
    };
}

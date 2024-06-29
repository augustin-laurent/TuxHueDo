#pragma once

#include <chrono>

namespace Huenicorn
{
  namespace Timing
  {
    using TimeScale = std::ratio<1, 1>;
    using TimeUnitType = double;
    using TimeUnit = std::chrono::duration<TimeUnitType, TimeScale>;
    using ClockType = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<ClockType, TimeUnit>;
    using Duration = TimeUnit;
  }
}

#ifndef COARSE_CLOCK_H
#define COARSE_CLOCK_H

#include <chrono>

namespace core {

  /// CoarseClock is similar to std::chrono::steady_clock, but operates at lower
  /// resolution (around 10-15ms on Windows, 1ms on Unix).  For understandable
  /// reasons, the STL implementations seems to choose to make steady_clock
  /// the same as high_resolution_clock, without providing a fast option for
  /// applications that are not especially sensitive to accuracy.

  struct CoarseClock {
    using rep = long long;
    using period = std::ratio<1, 10'000'000>;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<CoarseClock>;
    static constexpr bool is_steady = true;

    static time_point now() noexcept;
  };

}

#endif

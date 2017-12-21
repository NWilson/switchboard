#ifndef BASIC_TIMER_H
#define BASIC_TIMER_H

#include <chrono>

namespace core {

  template<class Clock>
  class BasicTimer {
  public:
    using executor_type = IoContext::executor_type;
    using clock_type = Clock;
    using duration = typename clock_type::duration;
    using time_point = typename clock_type::time_point;


    BasicTimer(IoContext& ctx, const time_point& t);
    BasicTimer(IoContext& ctx, const duration& d);
    BasicTimer(const BasicTimer&) = delete;
    BasicTimer(BasicTimer&& other);
    ~BasicTimer();
    BasicTimer& operator=(const BasicTimer&) = delete;
    BasicTimer& operator=(BasicTimer&& rhs);

    executor_type getExecutor() noexcept;
    time_point expiry() const;

    Future<void> wait();
    Future<void> operator co_await() { return wait(); }
  };

  // Could use custom timer with CLOCK_MONOTONIC_COARSE
  using Timer = BasicTimer<chrono::steady_clock>;

}

#endif

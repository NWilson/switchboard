#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <core/CoarseClock.h>
#include <core/ExecutionContext.h>

namespace core {

  /// A Scheduler provides services for delayed job execution.
  ///
  /// To schedule a job whose return value you wish to await, use a @ref Timer.

  template<class Clock>
  class Scheduler : public ExecutionContext::Service<Scheduler<Clock>> {
  public:
    using clock_type = Clock;
    using duration = clock_type::duration;
    using time_point = clock_type::time_point;

    Scheduler(ExecutionContext& ctx);

    template<class Func, class Duration>
    void scheduleAfter(Func&& f, Duration delay) const
    { return scheduleAt(std::move(f), clock_type::now() + delay); }

    template<class Func, class TimePoint>
    void scheduleAt(Func&& f, TimePoint t_) const
    {
      time_point t = time_cast<time_point, TimePoint>(t_);
      // XXX
    }

    // TODO: for a general timer, an invokable is always going to have an
    // allocation for the Func f.
    //   movable Invokable<Func>  ==>  unique_ptr<Wrapper<Func>>
    //   for "common" Func could have a specialisation Invokable which stores
    //   the movable Func without an allocation.
    //
    // For cancellation, we'd need:
    //  * shared state holding the PriQueue index
    //  * Q holds onto the state for fiddling it when the Q reorders
    //  * client uses Q mutex to protect access to Cancellable - the cancel
    //    method must lock on Q mutex and remove
    // So - can that be include in a Promise/timer *intrusively*?

  private:
    virtual void shutdown();

    template<typename DstTimePoint, typename SrcTimePoint>
    DstTimePoint time_cast(const SrcTimePoint& srcTime) {
      if (constexpr std::is_same<DstTimePoint, SrcTimePoint>)
        return srcTime;
      // Overkill? Oh well.
      const auto srcBefore = SrcTimePoint::clock::now();
      const auto dstNow = DstClock::now();
      const auto srcAfter = SrcTimePoint::clock::now();
      const auto srcBest = srcBefore + (srcAfter - srcBefore) / 2;
      return dstNow + (srcTime - srcBest);
    }
  };

  using CoarseScheduler = Scheduler<CoarseClock>;
  using HighResolutionScheduler = Scheduler<std::chrono::high_resolution_clock>;

}

#endif

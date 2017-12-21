#include <core/CoarseClock.h>

#ifdef WIN32
#include <assert.h>
#include <Windows.h>
#else
#include <time.h>
#endif

using namespace core;


CoarseClock::time_point CoarseClock::now() noexcept
{
#ifdef WIN32
  ULONGLONG unbiasedTime = 0;
  BOOL rv = QueryUnbiasedInterruptTime(&unbiasedTime);
  assert(!rv);
  return time_point{ duration{unbiasedTime} };
#else
  struct timespec ts = {};
  int rv = clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
  assert(rv == 0);
  return time_point{ duration{ rep{ts.tv_sec} * 10'000'000 +
                               ts.tv_nsec / 100 } };
#endif
}

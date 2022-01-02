#ifndef __BACKOFF_ALGO_H__
#define __BACKOFF_ALGO_H__

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <thread>

// Inspired by a blog on AWS named 'Exponential Backoff And Jitter'
// and the implementation of tikv.

#if __cplusplus >= 201703L
#define MY_CONSTEXPR constexpr
#else
#define MY_CONSTEXPR
#endif

namespace bf {

enum class BackoffStrategy : uint8_t {
  // NoJitter makes the backoff sequence strict exponential.
  kNoJitter,

  // FullJitter applies random factors to strict exponential.
  kFullJitter,

  // EqualJitter is also randomized, but prevents very short sleeps.
  kEqualJitter,

  // DecorrJitter increases the maximum jitter based on the last random value.
  kDecorrJitter,
};

namespace detail {
int exponential(int base, int cap, int n) {
  int x = static_cast<int>(std::ceil(base * std::pow(2.0, n)));
  return std::min(cap, x);
}
}  // namespace detail

template <uint64_t BASE, uint64_t CAP,
          BackoffStrategy ALGO = BackoffStrategy::kNoJitter>
struct BackoffFnCfg {
  uint64_t base{BASE};
  uint64_t cap{CAP};
  BackoffStrategy algo{ALGO};
};

template <uint64_t BASE, uint64_t CAP,
          BackoffStrategy ALGO = BackoffStrategy::kNoJitter>
class BackoffFunctor {
 public:
  BackoffFunctor()
      : last_sleep_(0),
        total_sleep_(0),
        attempts_(0),
        rng_engine_(std::random_device{}()) {}

  uint64_t operator()(uint64_t max_sleep_ms) {
    uint64_t sleep = 0;
    uint64_t v = detail::exponential(cfg_.base, cfg_.cap, attempts_);
    if MY_CONSTEXPR (ALGO == BackoffStrategy::kNoJitter) {
      sleep = v;
    } else if MY_CONSTEXPR (ALGO == BackoffStrategy::kFullJitter) {
      sleep = NextLong(v);
    } else if MY_CONSTEXPR (ALGO == BackoffStrategy::kEqualJitter) {
      sleep = v / 2 + NextLong(v / 2);
    } else if MY_CONSTEXPR (ALGO == BackoffStrategy::kDecorrJitter) {
      assert(last_sleep_ == 0 || ((last_sleep_ * 3) >= BASE));
      auto delta = last_sleep_ == 0 ? 0 : NextLong(last_sleep_ * 3 - BASE);
      sleep = std::min(CAP, BASE + delta);
    } else {
      sleep = BASE;  // Never reach
    }

    if (sleep > max_sleep_ms) {
      sleep = max_sleep_ms;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
    attempts_++;
    last_sleep_ = sleep;
    total_sleep_ += sleep;
    return last_sleep_;
  }

  uint64_t NumAttempts() const { return attempts_; }
  uint64_t TotalSleeps() const { return total_sleep_; }
  
 private:
  uint64_t NextLong(uint64_t x) {
    std::uniform_int_distribution<uint64_t> rng(0, x);
    return rng(rng_engine_);
  }
  uint64_t attempts_;
  uint64_t last_sleep_;
  uint64_t total_sleep_;
  BackoffFnCfg<BASE, CAP, ALGO> cfg_;
  std::default_random_engine rng_engine_;
};

}  // namespace bf

#endif
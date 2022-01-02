#include "backoff.h"

#include <iostream>

int main(int argc, char** argv) {
  std::cout << "no_jitter_backoff_fn:" << std::endl;
  bf::BackoffFunctor<200, 100000, bf::BackoffStrategy::kNoJitter>
      no_jitter_backoff_fn;
  std::cout << no_jitter_backoff_fn(100000) << std::endl;
  std::cout << no_jitter_backoff_fn(100000) << std::endl;
  std::cout << no_jitter_backoff_fn(100000) << std::endl;
  std::cout << no_jitter_backoff_fn(100000) << std::endl;
  std::cout << "NumAttempts:" << no_jitter_backoff_fn.NumAttempts()
            << ", TotalSleep:" << no_jitter_backoff_fn.TotalSleeps()
            << std::endl;

  std::cout << "full_jitter_backoff_fn:" << std::endl;
  bf::BackoffFunctor<200, 100000, bf::BackoffStrategy::kFullJitter>
      full_jitter_backoff_fn;
  std::cout << full_jitter_backoff_fn(100000) << std::endl;
  std::cout << full_jitter_backoff_fn(100000) << std::endl;
  std::cout << full_jitter_backoff_fn(100000) << std::endl;
  std::cout << full_jitter_backoff_fn(100000) << std::endl;

  std::cout << "eq_jitter_backoff_fn:" << std::endl;
  bf::BackoffFunctor<200, 100000, bf::BackoffStrategy::kEqualJitter>
      eq_jitter_backoff_fn;
  std::cout << eq_jitter_backoff_fn(100000) << std::endl;
  std::cout << eq_jitter_backoff_fn(100000) << std::endl;
  std::cout << eq_jitter_backoff_fn(100000) << std::endl;
  std::cout << eq_jitter_backoff_fn(100000) << std::endl;

  std::cout << "decorr_jitter_backoff_fn:" << std::endl;
  bf::BackoffFunctor<200, 100000, bf::BackoffStrategy::kDecorrJitter>
      decorr_jitter_backoff_fn;
  std::cout << decorr_jitter_backoff_fn(100000) << std::endl;
  std::cout << decorr_jitter_backoff_fn(100000) << std::endl;
  std::cout << decorr_jitter_backoff_fn(100000) << std::endl;
  std::cout << decorr_jitter_backoff_fn(100000) << std::endl;

  return 0;
}
#pragma once

#include <chrono>

namespace truegraphics::core {

class Time final {
 public:
  static double now_seconds();

 private:
  using Clock = std::chrono::steady_clock;
};

}  // namespace truegraphics::core

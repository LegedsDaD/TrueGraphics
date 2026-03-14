#include "truegraphics/core/Time.h"

namespace truegraphics::core {

double Time::now_seconds() {
  const auto now = Clock::now().time_since_epoch();
  return std::chrono::duration<double>(now).count();
}

}  // namespace truegraphics::core

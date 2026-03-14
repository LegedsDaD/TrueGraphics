#include "truegraphics/core/Uuid.h"

#include <atomic>
#include <sstream>

namespace truegraphics::core {

std::uint64_t Uuid::next_value() {
  static std::atomic<std::uint64_t> counter{1};
  return counter.fetch_add(1, std::memory_order_relaxed);
}

std::string Uuid::generate() {
  std::ostringstream oss;
  oss << "tg-" << next_value();
  return oss.str();
}

}  // namespace truegraphics::core

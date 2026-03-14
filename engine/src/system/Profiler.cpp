#include "truegraphics/system/Profiler.h"

#include <sstream>

#include "truegraphics/core/Time.h"

namespace truegraphics::system {

void Profiler::begin_frame() { frame_start_ = core::Time::now_seconds(); }

void Profiler::end_frame() { frame_ms_ = (core::Time::now_seconds() - frame_start_) * 1000.0; }

double Profiler::last_frame_ms() const { return frame_ms_; }

std::string Profiler::summary() const {
  std::ostringstream oss;
  oss << "frame=" << frame_ms_ << "ms";
  return oss.str();
}

}  // namespace truegraphics::system

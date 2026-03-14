#pragma once

#include <string>

namespace truegraphics::system {

class Profiler final {
 public:
  void begin_frame();
  void end_frame();
  double last_frame_ms() const;
  std::string summary() const;

 private:
  double frame_start_ = 0.0;
  double frame_ms_ = 0.0;
};

}  // namespace truegraphics::system

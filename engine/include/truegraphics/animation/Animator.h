#pragma once

#include <string>
#include <vector>

namespace truegraphics::animation {

struct Tween {
  std::string property;
  double start = 0.0;
  double end = 0.0;
  double duration = 0.0;
  double elapsed = 0.0;
  bool smoothstep = true;
};

class Animator final {
 public:
  void animate(std::string property, double start, double end, double duration, bool smoothstep = true);
  void update(double dt);
  double value(const std::string& property, double fallback) const;
  bool active(const std::string& property) const;
  void cleanup_finished();

 private:
  std::vector<Tween> tweens_;
};

}  // namespace truegraphics::animation

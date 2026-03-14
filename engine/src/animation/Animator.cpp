#include "truegraphics/animation/Animator.h"

#include <algorithm>

#include "truegraphics/animation/Easing.h"

namespace truegraphics::animation {

void Animator::animate(std::string property, double start, double end, double duration, bool smoothstep) {
  tweens_.push_back(Tween{std::move(property), start, end, duration, 0.0, smoothstep});
}

void Animator::update(double dt) {
  if (dt < 0.0) dt = 0.0;
  for (auto& t : tweens_) {
    t.elapsed += dt;
  }
}

double Animator::value(const std::string& property, double fallback) const {
  for (const auto& tween : tweens_) {
    if (tween.property == property) {
      double t = tween.duration <= 0.0 ? 1.0 : (tween.elapsed / tween.duration);
      t = std::clamp(t, 0.0, 1.0);
      if (tween.smoothstep) {
        t = Easing::smoothstep(t);
      }
      return tween.start + (tween.end - tween.start) * t;
    }
  }
  return fallback;
}

bool Animator::active(const std::string& property) const {
  for (const auto& tween : tweens_) {
    if (tween.property == property) {
      return tween.elapsed < tween.duration;
    }
  }
  return false;
}

void Animator::cleanup_finished() {
  tweens_.erase(
      std::remove_if(
          tweens_.begin(),
          tweens_.end(),
          [](const Tween& t) { return t.duration <= 0.0 || t.elapsed >= t.duration; }),
      tweens_.end());
}

}  // namespace truegraphics::animation

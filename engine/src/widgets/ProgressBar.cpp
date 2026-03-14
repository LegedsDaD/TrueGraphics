#include "truegraphics/widgets/ProgressBar.h"

#include <algorithm>

namespace truegraphics::widgets {

ProgressBar::ProgressBar(double value) : value_(value) {
  set_size(320, 22);
  style_.use_gradient = false;
  style_.corner_radius = 10;
  style_.border_width = 1;
}

double ProgressBar::value() const { return value_; }

void ProgressBar::set_value(double v) { value_ = std::clamp(v, 0.0, 1.0); }

void ProgressBar::draw(graphics::Renderer& renderer) {
  renderer.draw_rect(x_, y_, width_, height_, style_.background, style_.corner_radius);
  if (style_.border_width > 0) {
    renderer.draw_rect_outline(x_, y_, width_, height_, style_.border, style_.corner_radius, style_.border_width);
  }

  const int fill_w = static_cast<int>(width_ * std::clamp(value_, 0.0, 1.0));
  renderer.draw_rect(x_, y_, fill_w, height_, style_.foreground, style_.corner_radius);
}

}  // namespace truegraphics::widgets


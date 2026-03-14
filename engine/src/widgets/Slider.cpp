#include "truegraphics/widgets/Slider.h"

#include <algorithm>

namespace truegraphics::widgets {

Slider::Slider(double min, double max, double value) : min_(min), max_(max), value_(value) {
  set_size(320, 36);
  style_.use_gradient = false;
  style_.corner_radius = 10;
  style_.border_width = 1;
  set_focusable(true);

  set_on_mouse_down([this](int32_t mx, int32_t /*my*/) { update_from_mouse(mx); });
  set_on_mouse_move([this](int32_t mx, int32_t /*my*/) {
    if (focused_) {
      update_from_mouse(mx);
    }
  });
}

double Slider::value() const { return value_; }

void Slider::set_value(double v) {
  v = std::clamp(v, min_, max_);
  if (value_ == v) return;
  value_ = v;
  if (on_change_) on_change_(value_);
}

void Slider::set_on_change(std::function<void(double)> cb) { on_change_ = std::move(cb); }

void Slider::update_from_mouse(int32_t mx) {
  const int track_x = x_ + 10;
  const int track_w = std::max(1, width_ - 20);
  double t = static_cast<double>(mx - track_x) / static_cast<double>(track_w);
  t = std::clamp(t, 0.0, 1.0);
  set_value(min_ + (max_ - min_) * t);
}

void Slider::draw(graphics::Renderer& renderer) {
  const int track_x = x_ + 10;
  const int track_y = y_ + height_ / 2 - 3;
  const int track_w = std::max(1, width_ - 20);

  renderer.draw_rect(x_, y_, width_, height_, style_.background, style_.corner_radius);
  if (style_.border_width > 0) {
    renderer.draw_rect_outline(x_, y_, width_, height_, style_.border, style_.corner_radius, style_.border_width);
  }

  renderer.draw_rect(track_x, track_y, track_w, 6, {0, 0, 0, 60}, 3);

  double t = (max_ - min_) == 0.0 ? 0.0 : (value_ - min_) / (max_ - min_);
  t = std::clamp(t, 0.0, 1.0);
  const int thumb_x = track_x + static_cast<int>(t * track_w);
  renderer.draw_circle(thumb_x, y_ + height_ / 2, 8, style_.foreground, true);
}

}  // namespace truegraphics::widgets


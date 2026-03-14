#include "truegraphics/widgets/ScrollView.h"

#include <algorithm>

namespace truegraphics::widgets {

ScrollView::ScrollView() {
  set_size(520, 240);
  style_.use_gradient = false;
  style_.padding = 12;
  style_.corner_radius = 12;
  style_.border_width = 1;
}

void ScrollView::set_scroll_x(int x) {
  scroll_x_ = x;
  clamp_scroll();
}

int ScrollView::scroll_x() const { return scroll_x_; }

void ScrollView::scroll_by_x(int dx) {
  scroll_x_ += dx;
  clamp_scroll();
}

void ScrollView::set_scroll_y(int y) {
  scroll_y_ = y;
  clamp_scroll();
}

int ScrollView::scroll_y() const { return scroll_y_; }

void ScrollView::scroll_by(int dy) {
  scroll_y_ += dy;
  clamp_scroll();
}

int ScrollView::content_width() const {
  int right = x_;
  for (const auto& child : children_) {
    right = std::max(right, child->x() + child->width());
  }
  return std::max(0, right - x_);
}

int ScrollView::content_height() const {
  int bottom = y_;
  for (const auto& child : children_) {
    bottom = std::max(bottom, child->y() + child->height());
  }
  return std::max(0, bottom - y_);
}

int ScrollView::max_scroll_x() const { return std::max(0, content_width() - width_); }
int ScrollView::max_scroll_y() const { return std::max(0, content_height() - height_); }

void ScrollView::clamp_scroll() {
  scroll_x_ = std::clamp(scroll_x_, 0, max_scroll_x());
  scroll_y_ = std::clamp(scroll_y_, 0, max_scroll_y());
}

void ScrollView::draw(graphics::Renderer& renderer) {
  renderer.draw_rect(x_, y_, width_, height_, style_.background, style_.corner_radius);
  if (style_.border_width > 0) {
    renderer.draw_rect_outline(x_, y_, width_, height_, style_.border, style_.corner_radius, style_.border_width);
  }

  renderer.push_clip(x_, y_, width_, height_);
  renderer.push_offset(-scroll_x_, -scroll_y_);
  for (const auto& child : children_) {
    child->draw(renderer);
  }
  renderer.pop_offset();
  renderer.pop_clip();

  // simple scrollbar (right side)
  const int content_h = content_height();
  if (content_h > height_) {
    const int track_x = x_ + width_ - 8;
    const int track_y = y_ + 6;
    const int track_h = height_ - 12;

    const double ratio = static_cast<double>(height_) / static_cast<double>(content_h);
    const int thumb_h = std::max(20, static_cast<int>(track_h * ratio));
    const int max_scroll = max_scroll_y();
    const int thumb_y = max_scroll > 0 ? track_y + (track_h - thumb_h) * scroll_y_ / max_scroll : track_y;

    renderer.draw_rect(track_x, track_y, 4, track_h, {0, 0, 0, 40}, 2);
    renderer.draw_rect(track_x, thumb_y, 4, thumb_h, style_.border, 2);
  }

  // simple horizontal scrollbar (bottom)
  const int content_w = content_width();
  if (content_w > width_) {
    const int track_x = x_ + 6;
    const int track_y = y_ + height_ - 8;
    const int track_w = width_ - 12;

    const double ratio = static_cast<double>(width_) / static_cast<double>(content_w);
    const int thumb_w = std::max(20, static_cast<int>(track_w * ratio));
    const int max_scroll = max_scroll_x();
    const int thumb_x = max_scroll > 0 ? track_x + (track_w - thumb_w) * scroll_x_ / max_scroll : track_x;

    renderer.draw_rect(track_x, track_y, track_w, 4, {0, 0, 0, 40}, 2);
    renderer.draw_rect(thumb_x, track_y, thumb_w, 4, style_.border, 2);
  }
}

void ScrollView::add_child(const std::shared_ptr<Widget>& child) {
  Widget::add_child(child);
  clamp_scroll();
}

std::shared_ptr<Widget> ScrollView::hit_test_deep(int px, int py) {
  if (!hit_test(px, py)) {
    return nullptr;
  }

  // Children are drawn with an offset of (-scroll_x_, -scroll_y_), so adjust the query point.
  const int adjusted_x = px + scroll_x_;
  const int adjusted_y = py + scroll_y_;

  for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
    if (auto hit = (*it)->hit_test_deep(adjusted_x, adjusted_y)) {
      return hit;
    }
  }

  return shared_from_this();
}

}  // namespace truegraphics::widgets

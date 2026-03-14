#include "truegraphics/widgets/Dropdown.h"

#include <algorithm>

namespace truegraphics::widgets {

Dropdown::Dropdown(std::vector<std::string> items, int selected) : items_(std::move(items)), selected_(selected) {
  set_size(320, collapsed_h_);
  style_.use_gradient = false;
  style_.corner_radius = 10;
  style_.border_width = 1;
  set_focusable(true);

  set_on_mouse_down([this](int32_t mx, int32_t my) { handle_click(mx, my); });
}

int Dropdown::selected_index() const { return selected_; }

const std::string& Dropdown::selected_text() const {
  static const std::string empty;
  if (items_.empty()) return empty;
  const int idx = std::clamp(selected_, 0, static_cast<int>(items_.size() - 1));
  return items_[static_cast<size_t>(idx)];
}

void Dropdown::set_items(std::vector<std::string> items) {
  items_ = std::move(items);
  selected_ = std::clamp(selected_, 0, static_cast<int>(items_.empty() ? 0 : (items_.size() - 1)));
  if (open_) {
    set_size(width_, collapsed_h_ + static_cast<int>(items_.size()) * item_h_);
  }
}

void Dropdown::set_selected(int index) {
  const int next = std::clamp(index, 0, static_cast<int>(items_.empty() ? 0 : (items_.size() - 1)));
  if (selected_ == next) return;
  selected_ = next;
  if (on_change_) on_change_(selected_);
}

void Dropdown::set_on_change(std::function<void(int)> cb) { on_change_ = std::move(cb); }

void Dropdown::toggle_open() {
  open_ = !open_;
  if (open_) {
    set_size(width_, collapsed_h_ + static_cast<int>(items_.size()) * item_h_);
  } else {
    set_size(width_, collapsed_h_);
  }
}

void Dropdown::handle_click(int32_t mx, int32_t my) {
  (void)mx;
  if (!open_) {
    toggle_open();
    return;
  }

  // click in list area?
  const int list_y = y_ + collapsed_h_;
  if (my >= list_y) {
    const int idx = (my - list_y) / item_h_;
    if (idx >= 0 && idx < static_cast<int>(items_.size())) {
      set_selected(idx);
    }
  }

  toggle_open();
}

void Dropdown::draw(graphics::Renderer& renderer) {
  renderer.draw_rect(x_, y_, width_, height_, style_.background, style_.corner_radius);
  if (style_.border_width > 0) {
    renderer.draw_rect_outline(x_, y_, width_, height_, style_.border, style_.corner_radius, style_.border_width);
  }

  renderer.draw_text(x_ + 12, y_ + 10, selected_text(), style_.foreground, style_.font_family, style_.font_size);

  if (!open_) return;

  const int list_y = y_ + collapsed_h_;
  for (int i = 0; i < static_cast<int>(items_.size()); ++i) {
    const int iy = list_y + i * item_h_;
    if (i == selected_) {
      renderer.draw_rect(x_ + 4, iy, width_ - 8, item_h_, {0, 0, 0, 60}, 6);
    }
    renderer.draw_text(x_ + 12, iy + 6, items_[static_cast<size_t>(i)], style_.foreground, style_.font_family, style_.font_size);
  }
}

}  // namespace truegraphics::widgets

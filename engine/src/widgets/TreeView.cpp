#include "truegraphics/widgets/TreeView.h"

#include <algorithm>

namespace truegraphics::widgets {

TreeView::TreeView(std::vector<Item> items) : items_(std::move(items)) {
  set_size(520, 240);
  style_.use_gradient = false;
  style_.border_width = 1;
  style_.corner_radius = 10;
  set_focusable(true);
  set_on_mouse_down([this](int32_t mx, int32_t my) { handle_click(mx, my); });
}

void TreeView::set_items(std::vector<Item> items) {
  items_ = std::move(items);
  selected_ = std::clamp(selected_, -1, static_cast<int>(items_.size()) - 1);
}

int TreeView::selected_index() const { return selected_; }

void TreeView::set_selected(int idx) {
  const int next = std::clamp(idx, -1, static_cast<int>(items_.size()) - 1);
  if (selected_ == next) return;
  selected_ = next;
  if (on_select_) on_select_(selected_);
}

void TreeView::set_on_select(std::function<void(int)> cb) { on_select_ = std::move(cb); }

void TreeView::handle_click(int32_t /*mx*/, int32_t my) {
  const int rel_y = my - y_ - style_.padding;
  if (rel_y < 0) return;
  const int idx = rel_y / item_h_;
  if (idx >= 0 && idx < static_cast<int>(items_.size())) {
    set_selected(idx);
  }
}

void TreeView::draw(graphics::Renderer& renderer) {
  renderer.draw_rect(x_, y_, width_, height_, style_.background, style_.corner_radius);
  if (style_.border_width > 0) {
    renderer.draw_rect_outline(x_, y_, width_, height_, style_.border, style_.corner_radius, style_.border_width);
  }

  const int start_y = y_ + style_.padding;
  for (int i = 0; i < static_cast<int>(items_.size()); ++i) {
    const int iy = start_y + i * item_h_;
    if (iy + item_h_ < y_ || iy > y_ + height_) continue;
    if (i == selected_) {
      renderer.draw_rect(x_ + 4, iy, width_ - 8, item_h_, {0, 0, 0, 60}, 6);
    }

    const int indent = 16 * std::max(0, items_[static_cast<size_t>(i)].level);
    renderer.draw_text(x_ + 12 + indent, iy + 5, items_[static_cast<size_t>(i)].text, style_.foreground, style_.font_family, style_.font_size);
  }
}

}  // namespace truegraphics::widgets

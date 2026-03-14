#include "truegraphics/widgets/Tabs.h"

#include <algorithm>

namespace truegraphics::widgets {

Tabs::Tabs() {
  set_size(520, 260);
  style_.use_gradient = false;
  style_.corner_radius = 12;
  style_.border_width = 1;
  style_.padding = 10;

  set_focusable(true);
  set_on_mouse_down([this](int32_t mx, int32_t my) { handle_header_click(mx, my); });
}

std::shared_ptr<Container> Tabs::tab(const std::string& title) {
  auto page = std::make_shared<Container>();
  page->set_layout(Container::LayoutMode::Column);
  page->set_gap(10);
  page->style().border_width = 0;
  page->style().corner_radius = 0;
  page->style().use_gradient = false;
  add_child(page);
  tabs_.push_back(Tab{title, page});
  return page;
}

int Tabs::active_index() const { return active_; }

void Tabs::set_active_index(int idx) {
  if (tabs_.empty()) {
    active_ = 0;
    return;
  }
  active_ = std::clamp(idx, 0, static_cast<int>(tabs_.size() - 1));
}

void Tabs::handle_header_click(int32_t mx, int32_t my) {
  if (tabs_.empty()) return;

  if (my < y_ || my > y_ + header_h_) return;
  if (mx < x_ || mx > x_ + width_) return;

  const int tab_w = std::max(60, width_ / static_cast<int>(tabs_.size()));
  const int idx = (mx - x_) / tab_w;
  set_active_index(idx);
}

void Tabs::draw(graphics::Renderer& renderer) {
  renderer.draw_rect(x_, y_, width_, height_, style_.background, style_.corner_radius);
  if (style_.border_width > 0) {
    renderer.draw_rect_outline(x_, y_, width_, height_, style_.border, style_.corner_radius, style_.border_width);
  }

  // header
  renderer.draw_rect(x_, y_, width_, header_h_, {0, 0, 0, 40}, style_.corner_radius);

  if (!tabs_.empty()) {
    const int tab_w = std::max(60, width_ / static_cast<int>(tabs_.size()));
    for (int i = 0; i < static_cast<int>(tabs_.size()); ++i) {
      const int tx = x_ + i * tab_w;
      if (i == active_) {
        renderer.draw_rect(tx, y_, tab_w, header_h_, {0, 0, 0, 70}, style_.corner_radius);
      }
      renderer.draw_text(tx + 10, y_ + 10, tabs_[static_cast<size_t>(i)].title, style_.foreground, style_.font_family, style_.font_size);
    }

    // active page
    active_ = std::clamp(active_, 0, static_cast<int>(tabs_.size() - 1));
    auto& page = tabs_[static_cast<size_t>(active_)].page;
    if (page) {
      const int px = x_ + style_.padding;
      const int py = y_ + header_h_ + style_.padding;
      const int pw = std::max(0, width_ - style_.padding * 2);
      const int ph = std::max(0, height_ - header_h_ - style_.padding * 2);
      page->set_bounds(px, py, pw, ph);
      page->draw(renderer);
    }
  }
}

}  // namespace truegraphics::widgets

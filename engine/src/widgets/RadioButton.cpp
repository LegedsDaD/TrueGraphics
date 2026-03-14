#include "truegraphics/widgets/RadioButton.h"

#include <algorithm>
#include <memory>

namespace truegraphics::widgets {

RadioButton::RadioButton(const std::string& text, std::string group, bool checked) : group_(std::move(group)) {
  set_text(text);
  set_size(220, 28);
  style_.use_gradient = false;
  set_focusable(true);
  set_checked(checked);
  set_on_click([this]() { select(); });
}

const std::string& RadioButton::group() const { return group_; }
void RadioButton::set_group(std::string group) { group_ = std::move(group); }

bool RadioButton::checked() const { return checked_; }

void RadioButton::set_checked(bool checked) {
  if (checked_ == checked) return;
  checked_ = checked;
  if (on_change_) on_change_(checked_);
}

void RadioButton::set_on_change(std::function<void(bool)> cb) { on_change_ = std::move(cb); }

void RadioButton::select() {
  auto p = parent();
  if (p) {
    for (const auto& child : p->children()) {
      if (!child) continue;
      if (child.get() == this) continue;
      auto rb = std::dynamic_pointer_cast<RadioButton>(child);
      if (rb && rb->group() == group_) {
        rb->set_checked(false);
      }
    }
  }
  set_checked(true);
}

void RadioButton::draw(graphics::Renderer& renderer) {
  const int r = 8;
  const int bx = x_;
  const int by = y_ + (height_ - r * 2) / 2;

  renderer.draw_circle(bx + r, by + r, r, style_.background, true);
  if (style_.border_width > 0) {
    renderer.draw_circle(bx + r, by + r, r, style_.border, false, style_.border_width);
  }
  if (checked_) {
    renderer.draw_circle(bx + r, by + r, r - 3, style_.foreground, true);
  }

  renderer.draw_text(x_ + r * 2 + 10, y_ + 6, text_, style_.foreground, style_.font_family, style_.font_size);
}

}  // namespace truegraphics::widgets

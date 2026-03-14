#include "truegraphics/widgets/Checkbox.h"

namespace truegraphics::widgets {

Checkbox::Checkbox(const std::string& text) {
  set_text(text);
  set_size(220, 28);
  style_.use_gradient = false;
}

bool Checkbox::checked() const { return checked_; }

void Checkbox::set_checked(bool checked) {
  checked_ = checked;
  if (on_change_) {
    on_change_(checked_);
  }
}

void Checkbox::set_on_change(std::function<void(bool)> cb) { on_change_ = std::move(cb); }

void Checkbox::draw(graphics::Renderer& renderer) {
  const int box = 18;
  const int bx = x_;
  const int by = y_ + (height_ - box) / 2;

  renderer.draw_rect(bx, by, box, box, style_.background, 4);
  if (style_.border_width > 0) {
    renderer.draw_rect_outline(bx, by, box, box, style_.border, 4, style_.border_width);
  }

  if (checked_) {
    renderer.draw_line(bx + 4, by + 9, bx + 8, by + 13, style_.foreground, 2);
    renderer.draw_line(bx + 8, by + 13, bx + 14, by + 5, style_.foreground, 2);
  }

  renderer.draw_text(x_ + box + 10, y_ + 6, text_, style_.foreground, style_.font_family, style_.font_size);
}

}  // namespace truegraphics::widgets

#include "truegraphics/widgets/Button.h"

namespace truegraphics::widgets {

Button::Button(const std::string& text) {
  set_text(text);
  set_size(220, 42);
}

void Button::draw(graphics::Renderer& renderer) {
  const auto fill = style_.use_gradient ? style_.gradient.start : style_.background;
  renderer.draw_rect(x_, y_, width_, height_, fill, style_.corner_radius);
  if (style_.border_width > 0) {
    renderer.draw_rect_outline(x_, y_, width_, height_, style_.border, style_.corner_radius, style_.border_width);
  }
  renderer.draw_text(x_ + 14, y_ + 12, text_, style_.foreground, style_.font_family, style_.font_size);
}

}  // namespace truegraphics::widgets

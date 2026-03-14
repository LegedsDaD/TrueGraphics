#include "truegraphics/widgets/Canvas.h"

#include <type_traits>

namespace truegraphics::widgets {

Canvas::Canvas() { set_size(320, 200); }

void Canvas::draw(graphics::Renderer& renderer) {
  renderer.draw_rect(x_, y_, width_, height_, {28, 28, 34, 255}, style_.corner_radius);
  if (style_.border_width > 0) {
    renderer.draw_rect_outline(x_, y_, width_, height_, style_.border, style_.corner_radius, style_.border_width);
  }

  for (const auto& cmd : commands_) {
    std::visit(
        [&](const auto& c) {
          using T = std::decay_t<decltype(c)>;
          if constexpr (std::is_same_v<T, LineCmd>) {
            renderer.draw_line(x_ + c.x1, y_ + c.y1, x_ + c.x2, y_ + c.y2, c.color, c.thickness);
          } else if constexpr (std::is_same_v<T, RectCmd>) {
            if (c.filled) {
              renderer.draw_rect(x_ + c.x, y_ + c.y, c.w, c.h, c.color, c.radius);
            } else {
              renderer.draw_rect_outline(x_ + c.x, y_ + c.y, c.w, c.h, c.color, c.radius, c.thickness);
            }
          } else if constexpr (std::is_same_v<T, CircleCmd>) {
            renderer.draw_circle(x_ + c.cx, y_ + c.cy, c.radius, c.color, c.filled, c.thickness);
          } else if constexpr (std::is_same_v<T, TextCmd>) {
            renderer.draw_text(x_ + c.x, y_ + c.y, c.text, c.color, style_.font_family, style_.font_size);
          }
        },
        cmd);
  }
}

void Canvas::clear_commands() { commands_.clear(); }

void Canvas::draw_line(int x1, int y1, int x2, int y2, graphics::Color color, int thickness) {
  commands_.push_back(LineCmd{x1, y1, x2, y2, thickness, color});
}

void Canvas::draw_rect_cmd(int x, int y, int w, int h, graphics::Color color, int radius, bool filled, int thickness) {
  commands_.push_back(RectCmd{x, y, w, h, radius, filled, thickness, color});
}

void Canvas::draw_circle(int cx, int cy, int radius, graphics::Color color, bool filled, int thickness) {
  commands_.push_back(CircleCmd{cx, cy, radius, filled, thickness, color});
}

void Canvas::draw_text_cmd(int x, int y, std::string text, graphics::Color color) {
  commands_.push_back(TextCmd{x, y, std::move(text), color});
}

}  // namespace truegraphics::widgets

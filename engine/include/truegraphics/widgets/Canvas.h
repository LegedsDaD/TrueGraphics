#pragma once

#include <string>
#include <variant>
#include <vector>

#include "truegraphics/graphics/Renderer.h"
#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class Canvas final : public Widget {
 public:
  Canvas();
  void draw(graphics::Renderer& renderer) override;

  void clear_commands();

  void draw_line(int x1, int y1, int x2, int y2, graphics::Color color, int thickness = 1);
  void draw_rect_cmd(int x, int y, int w, int h, graphics::Color color, int radius = 0, bool filled = true, int thickness = 1);
  void draw_circle(int cx, int cy, int radius, graphics::Color color, bool filled = true, int thickness = 1);
  void draw_text_cmd(int x, int y, std::string text, graphics::Color color);

 private:
  struct LineCmd {
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    int thickness = 1;
    graphics::Color color{};
  };
  struct RectCmd {
    int x = 0, y = 0, w = 0, h = 0;
    int radius = 0;
    bool filled = true;
    int thickness = 1;
    graphics::Color color{};
  };
  struct CircleCmd {
    int cx = 0, cy = 0, radius = 0;
    bool filled = true;
    int thickness = 1;
    graphics::Color color{};
  };
  struct TextCmd {
    int x = 0, y = 0;
    std::string text;
    graphics::Color color{};
  };

  using Command = std::variant<LineCmd, RectCmd, CircleCmd, TextCmd>;
  std::vector<Command> commands_;
};

}  // namespace truegraphics::widgets

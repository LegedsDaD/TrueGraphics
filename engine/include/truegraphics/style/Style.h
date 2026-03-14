#pragma once

#include <string>

#include "truegraphics/graphics/Renderer.h"

namespace truegraphics::style {

struct Shadow {
  int offset_x = 0;
  int offset_y = 4;
  int blur = 12;
  graphics::Color color{0, 0, 0, 96};
};

struct Gradient {
  graphics::Color start{48, 48, 56, 255};
  graphics::Color end{32, 32, 40, 255};
};

struct Style {
  graphics::Color background{45, 45, 52, 255};
  graphics::Color foreground{236, 236, 240, 255};
  graphics::Color border{90, 90, 100, 255};
  int corner_radius = 12;
  int border_width = 1;
  int padding = 10;
  int margin = 0;
  bool use_gradient = true;
  bool blur = false;
  bool dark_theme = true;
  Gradient gradient{};
  Shadow shadow{};
  std::string font_family = "Segoe UI";
  int font_size = 16;
};

}  // namespace truegraphics::style

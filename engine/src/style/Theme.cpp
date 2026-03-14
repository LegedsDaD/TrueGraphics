#include "truegraphics/style/Theme.h"

namespace truegraphics::style {

Style Theme::dark() {
  Style s;
  s.background = {40, 44, 52, 255};
  s.foreground = {235, 235, 240, 255};
  s.border = {80, 86, 96, 255};
  s.corner_radius = 12;
  s.shadow = {0, 6, 14, {0, 0, 0, 96}};
  s.gradient = {{58, 62, 72, 255}, {34, 36, 43, 255}};
  s.use_gradient = true;
  s.blur = true;
  s.dark_theme = true;
  return s;
}

Style Theme::light() {
  Style s;
  s.background = {245, 246, 250, 255};
  s.foreground = {24, 26, 31, 255};
  s.border = {200, 204, 214, 255};
  s.corner_radius = 12;
  s.shadow = {0, 6, 14, {0, 0, 0, 48}};
  s.gradient = {{255, 255, 255, 255}, {235, 237, 244, 255}};
  s.use_gradient = true;
  s.blur = false;
  s.dark_theme = false;
  return s;
}

}  // namespace truegraphics::style

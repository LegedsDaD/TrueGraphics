#pragma once

#include <cstdint>
#include <string>

namespace truegraphics::core {

struct Config {
  std::string title = "TrueGraphics";
  int32_t width = 960;
  int32_t height = 640;
  bool resizable = true;
  bool dark_theme = true;

  // Window features
  bool fullscreen = false;
  int32_t min_width = 0;
  int32_t min_height = 0;
  int32_t max_width = 0;
  int32_t max_height = 0;
  std::string icon_path;
};

}  // namespace truegraphics::core

#pragma once

#include "truegraphics/style/Style.h"

namespace truegraphics::style {

class Theme final {
 public:
  static Style dark();
  static Style light();
};

}  // namespace truegraphics::style

#pragma once

#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class PasswordBox final : public Widget {
 public:
  explicit PasswordBox(const std::string& text = "");
  void draw(graphics::Renderer& renderer) override;

 private:
  int caret_ = 0;
  int sel_anchor_ = 0;
  int sel_start_ = 0;
  int sel_end_ = 0;
};

}  // namespace truegraphics::widgets

#pragma once

#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class Button final : public Widget {
 public:
  explicit Button(const std::string& text = "");
  void draw(graphics::Renderer& renderer) override;
};

}  // namespace truegraphics::widgets

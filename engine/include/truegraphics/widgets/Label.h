#pragma once

#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class Label final : public Widget {
 public:
  explicit Label(const std::string& text = "");
  void draw(graphics::Renderer& renderer) override;
};

}  // namespace truegraphics::widgets

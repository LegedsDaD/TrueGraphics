#pragma once

#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class ProgressBar final : public Widget {
 public:
  explicit ProgressBar(double value = 0.0);
  double value() const;
  void set_value(double v);

  void draw(graphics::Renderer& renderer) override;

 private:
  double value_ = 0.0;
};

}  // namespace truegraphics::widgets


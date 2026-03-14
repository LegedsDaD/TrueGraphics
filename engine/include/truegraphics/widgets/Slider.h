#pragma once

#include <functional>

#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class Slider final : public Widget {
 public:
  Slider(double min = 0.0, double max = 1.0, double value = 0.0);

  double value() const;
  void set_value(double v);
  void set_on_change(std::function<void(double)> cb);

  void draw(graphics::Renderer& renderer) override;

 private:
  void update_from_mouse(int32_t x);
  double min_ = 0.0;
  double max_ = 1.0;
  double value_ = 0.0;
  std::function<void(double)> on_change_;
};

}  // namespace truegraphics::widgets


#pragma once

#include <functional>
#include <string>

#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class Checkbox final : public Widget {
 public:
  explicit Checkbox(const std::string& text = "");

  bool checked() const;
  void set_checked(bool checked);

  void set_on_change(std::function<void(bool)> cb);

  void draw(graphics::Renderer& renderer) override;

 private:
  bool checked_ = false;
  std::function<void(bool)> on_change_;
};

}  // namespace truegraphics::widgets


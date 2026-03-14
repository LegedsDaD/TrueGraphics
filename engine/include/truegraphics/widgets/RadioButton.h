#pragma once

#include <functional>
#include <string>

#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class RadioButton final : public Widget {
 public:
  RadioButton(const std::string& text = "", std::string group = "default", bool checked = false);

  const std::string& group() const;
  void set_group(std::string group);

  bool checked() const;
  void set_checked(bool checked);

  void set_on_change(std::function<void(bool)> cb);

  void draw(graphics::Renderer& renderer) override;

 private:
  void select();

  std::string group_ = "default";
  bool checked_ = false;
  std::function<void(bool)> on_change_;
};

}  // namespace truegraphics::widgets


#pragma once

#include <memory>

#include "truegraphics/style/Style.h"

namespace truegraphics::widgets {
class Widget;
}

namespace truegraphics::style {

class StyleEngine final {
 public:
  void set_theme(const Style& theme);
  void apply_defaults(const std::shared_ptr<widgets::Widget>& widget) const;

 private:
  Style theme_{};
};

}  // namespace truegraphics::style

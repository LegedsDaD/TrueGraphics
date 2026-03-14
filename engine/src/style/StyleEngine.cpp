#include "truegraphics/style/StyleEngine.h"

#include "truegraphics/style/Theme.h"
#include "truegraphics/widgets/Widget.h"

namespace truegraphics::style {

void StyleEngine::set_theme(const Style& theme) { theme_ = theme; }

void StyleEngine::apply_defaults(const std::shared_ptr<widgets::Widget>& widget) const {
  if (!widget) {
    return;
  }
  widget->style() = theme_;
}

}  // namespace truegraphics::style

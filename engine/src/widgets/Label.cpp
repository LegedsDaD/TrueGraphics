#include "truegraphics/widgets/Label.h"

namespace truegraphics::widgets {

Label::Label(const std::string& text) {
  set_text(text);
  set_size(220, 28);
}

void Label::draw(graphics::Renderer& renderer) {
  renderer.draw_text(x_, y_, text_, style_.foreground, style_.font_family, style_.font_size);
}

}  // namespace truegraphics::widgets

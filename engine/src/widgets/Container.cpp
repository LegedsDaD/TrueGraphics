#include "truegraphics/widgets/Container.h"

namespace truegraphics::widgets {

Container::Container() {
  set_size(900, 600);
  style_.padding = 20;
}

void Container::draw(graphics::Renderer& renderer) {
  renderer.draw_rect(x_, y_, width_, height_, style_.background, 0);
  for (const auto& child : children_) {
    child->draw(renderer);
  }
}

void Container::set_layout(LayoutMode mode) { layout_ = mode; }
Container::LayoutMode Container::layout_mode() const { return layout_; }

void Container::set_align(Align align) { align_ = align; }
Container::Align Container::align() const { return align_; }

void Container::set_justify(Justify justify) { justify_ = justify; }
Container::Justify Container::justify() const { return justify_; }

void Container::set_gap(int gap) { gap_ = gap; }
int Container::gap() const { return gap_; }

void Container::set_grid_columns(int columns) { grid_columns_ = columns > 0 ? columns : 1; }
int Container::grid_columns() const { return grid_columns_; }

}  // namespace truegraphics::widgets

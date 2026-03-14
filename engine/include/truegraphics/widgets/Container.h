#pragma once

#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class Container final : public Widget {
 public:
  enum class LayoutMode {
    None,
    Column,
    Row,
    Grid,
  };

  enum class Align {
    Start,
    Center,
    End,
    Stretch,
  };

  enum class Justify {
    Start,
    Center,
    End,
    SpaceBetween,
  };

  Container();
  void draw(graphics::Renderer& renderer) override;

  void set_layout(LayoutMode mode);
  LayoutMode layout_mode() const;

  void set_align(Align align);
  Align align() const;

  void set_justify(Justify justify);
  Justify justify() const;

  void set_gap(int gap);
  int gap() const;

  void set_grid_columns(int columns);
  int grid_columns() const;

 private:
  LayoutMode layout_ = LayoutMode::Column;
  Align align_ = Align::Start;
  Justify justify_ = Justify::Start;
  int gap_ = 12;
  int grid_columns_ = 2;
};

}  // namespace truegraphics::widgets

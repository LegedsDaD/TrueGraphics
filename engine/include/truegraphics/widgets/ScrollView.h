#pragma once

#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class ScrollView final : public Widget {
 public:
  ScrollView();

  void set_scroll_x(int x);
  int scroll_x() const;
  void scroll_by_x(int dx);

  void set_scroll_y(int y);
  int scroll_y() const;
  void scroll_by(int dy);

  void draw(graphics::Renderer& renderer) override;
  void add_child(const std::shared_ptr<Widget>& child) override;
  std::shared_ptr<Widget> hit_test_deep(int px, int py) override;

 private:
  int max_scroll_x() const;
  int content_width() const;
  int max_scroll_y() const;
  int content_height() const;
  void clamp_scroll();

  int scroll_x_ = 0;
  int scroll_y_ = 0;
};

}  // namespace truegraphics::widgets

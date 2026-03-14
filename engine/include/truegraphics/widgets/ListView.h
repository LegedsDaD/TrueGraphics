#pragma once

#include <functional>
#include <string>
#include <vector>

#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class ListView final : public Widget {
 public:
  explicit ListView(std::vector<std::string> items = {});

  void set_items(std::vector<std::string> items);
  int selected_index() const;
  void set_selected(int idx);
  void set_on_select(std::function<void(int)> cb);

  void draw(graphics::Renderer& renderer) override;

 private:
  void handle_click(int32_t mx, int32_t my);

  std::vector<std::string> items_;
  int selected_ = -1;
  int item_h_ = 24;
  std::function<void(int)> on_select_;
};

}  // namespace truegraphics::widgets


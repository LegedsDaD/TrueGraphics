#pragma once

#include <functional>
#include <string>
#include <vector>

#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class TreeView final : public Widget {
 public:
  struct Item {
    std::string text;
    int level = 0;
  };

  explicit TreeView(std::vector<Item> items = {});

  void set_items(std::vector<Item> items);
  int selected_index() const;
  void set_selected(int idx);
  void set_on_select(std::function<void(int)> cb);

  void draw(graphics::Renderer& renderer) override;

 private:
  void handle_click(int32_t mx, int32_t my);

  std::vector<Item> items_;
  int selected_ = -1;
  int item_h_ = 24;
  std::function<void(int)> on_select_;
};

}  // namespace truegraphics::widgets


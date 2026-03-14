#pragma once

#include <functional>
#include <string>
#include <vector>

#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class Dropdown final : public Widget {
 public:
  Dropdown(std::vector<std::string> items = {}, int selected = 0);

  int selected_index() const;
  const std::string& selected_text() const;
  void set_items(std::vector<std::string> items);
  void set_selected(int index);
  void set_on_change(std::function<void(int)> cb);

  void draw(graphics::Renderer& renderer) override;

 private:
  void toggle_open();
  void handle_click(int32_t mx, int32_t my);

  std::vector<std::string> items_;
  int selected_ = 0;
  bool open_ = false;
  int collapsed_h_ = 36;
  int item_h_ = 28;
  std::function<void(int)> on_change_;
};

}  // namespace truegraphics::widgets


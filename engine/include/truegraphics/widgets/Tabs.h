#pragma once

#include <string>
#include <vector>

#include "truegraphics/widgets/Container.h"
#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class Tabs final : public Widget {
 public:
  Tabs();

  std::shared_ptr<Container> tab(const std::string& title);
  int active_index() const;
  void set_active_index(int idx);

  void draw(graphics::Renderer& renderer) override;

 private:
  struct Tab {
    std::string title;
    std::shared_ptr<Container> page;
  };

  void handle_header_click(int32_t mx, int32_t my);

  std::vector<Tab> tabs_;
  int active_ = 0;
  int header_h_ = 36;
};

}  // namespace truegraphics::widgets


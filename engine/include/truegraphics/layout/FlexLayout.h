#pragma once

#include "truegraphics/layout/Layout.h"

namespace truegraphics::layout {

class FlexLayout final : public Layout {
 public:
  void apply(const std::shared_ptr<widgets::Container>& root) override;

 private:
  void apply_widget(const std::shared_ptr<widgets::Widget>& widget);
  void layout_container(const std::shared_ptr<widgets::Container>& container);
};

}  // namespace truegraphics::layout

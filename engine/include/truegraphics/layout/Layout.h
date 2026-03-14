#pragma once

#include <memory>

namespace truegraphics::widgets {
class Widget;
class Container;
}

namespace truegraphics::layout {

class Layout {
 public:
  virtual ~Layout() = default;
  virtual void apply(const std::shared_ptr<widgets::Container>& root) = 0;
};

}  // namespace truegraphics::layout

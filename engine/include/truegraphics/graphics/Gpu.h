#pragma once

#include <string>

namespace truegraphics::graphics {

class Gpu final {
 public:
  std::string backend_name() const;
};

}  // namespace truegraphics::graphics

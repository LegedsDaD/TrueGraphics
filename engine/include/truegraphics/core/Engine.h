#pragma once

#include <memory>
#include <vector>

#include "truegraphics/core/Config.h"

namespace truegraphics::core {
class App;
}

namespace truegraphics::graphics {
class Renderer;
}

namespace truegraphics::window {
class Window;
}

namespace truegraphics::core {

class Engine final {
 public:
  Engine();
  ~Engine();

  bool initialize(App& app);
  int run(App& app);
  void shutdown();

  window::Window* window(size_t index = 0);
  graphics::Renderer* renderer(size_t index = 0);

 private:
  std::vector<std::unique_ptr<window::Window>> windows_;
  std::vector<std::unique_ptr<graphics::Renderer>> renderers_;
  bool initialized_ = false;
};

}  // namespace truegraphics::core

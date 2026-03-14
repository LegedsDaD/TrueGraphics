#include "truegraphics/core/Engine.h"

#include <algorithm>
#include <mutex>
#include <utility>

#include <windows.h>

#include "truegraphics/core/App.h"
#include "truegraphics/core/Time.h"
#include "truegraphics/graphics/Renderer.h"
#include "truegraphics/window/Window.h"

namespace truegraphics::core {

Engine::Engine() = default;
Engine::~Engine() { shutdown(); }

bool Engine::initialize(App& app) {
  shutdown();

  const size_t count = std::max<size_t>(1, app.window_count());
  windows_.reserve(count);
  renderers_.reserve(count);

  for (size_t i = 0; i < count; ++i) {
    auto w = std::make_unique<window::Window>();
    if (!w->create(app.window_config(i))) {
      return false;
    }

    auto r = std::make_unique<graphics::Renderer>();
    if (!r->initialize(w.get())) {
      return false;
    }

    windows_.push_back(std::move(w));
    renderers_.push_back(std::move(r));
  }

  if (!windows_.empty() && app.tray_enabled()) {
    windows_[0]->enable_tray_icon(
        app.tray_icon_path(),
        app.tray_tooltip(),
        [&app]() { app.tray_click(); });
  }

  initialized_ = true;
  return true;
}

int Engine::run(App& app) {
  if (!initialized_) {
    if (!initialize(app)) {
      return -1;
    }
  }

  double last_tick_time = core::Time::now_seconds();
  std::mutex tick_mutex;

  for (size_t i = 0; i < windows_.size(); ++i) {
    auto* win = windows_[i].get();
    auto* ren = renderers_[i].get();

    win->set_tick([ren, &app, &last_tick_time, &tick_mutex, i](void* paint_dc) {
      const double now = core::Time::now_seconds();
      double dt = 0.0;
      {
        std::lock_guard<std::mutex> lock(tick_mutex);
        dt = now - last_tick_time;
        if (dt > 0.0005) {
          last_tick_time = now;
        } else {
          dt = 0.0;
        }
      }

      if (dt > 0.0) {
        app.tick(dt);
      }

      ren->begin_frame(paint_dc);
      ren->clear({24, 24, 28, 255});
      app.render(*ren, i);
      ren->end_frame();
    });

    win->set_mouse_left([&app, i](int x, int y, bool pressed) { app.on_mouse_left(x, y, pressed, i); });
    win->set_mouse_move([&app, i](int32_t x, int32_t y) { app.on_mouse_move(x, y, i); });
    win->set_mouse_wheel([&app, i](int32_t x, int32_t y, int32_t delta) { app.on_mouse_wheel(x, y, delta, i); });
    win->set_key_down([&app, i](int32_t key) { app.on_key_down(key, i); });
    win->set_char([&app, i](uint32_t codepoint) { app.on_char(codepoint, i); });
    win->set_drop_files([&app, i](int32_t x, int32_t y, std::vector<std::string> files) { app.on_drop_files(x, y, std::move(files), i); });
    win->set_resize([&app, i](int32_t w, int32_t h) { app.on_resize(w, h, i); });
  }

  for (auto& w : windows_) {
    w->show();
  }

  MSG msg{};
  while (true) {
    int res = GetMessageW(&msg, nullptr, 0, 0);
    if (res == 0) {
      return static_cast<int>(msg.wParam);
    }
    if (res == -1) {
      return -1;
    }
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
}

void Engine::shutdown() {
  renderers_.clear();
  for (auto& w : windows_) {
    if (w) w->close();
  }
  windows_.clear();
  initialized_ = false;
}

window::Window* Engine::window(size_t index) {
  if (index >= windows_.size()) return nullptr;
  return windows_[index].get();
}

graphics::Renderer* Engine::renderer(size_t index) {
  if (index >= renderers_.size()) return nullptr;
  return renderers_[index].get();
}

}  // namespace truegraphics::core

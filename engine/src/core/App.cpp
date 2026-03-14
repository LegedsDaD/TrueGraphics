#include "truegraphics/core/App.h"

#include <algorithm>
#include <functional>
#include <memory>

#include <windows.h>

#include "truegraphics/core/Engine.h"
#include "truegraphics/resource/Resources.h"
#include "truegraphics/style/Theme.h"
#include "truegraphics/widgets/Container.h"
#include "truegraphics/widgets/ScrollView.h"
#include "truegraphics/widgets/Widget.h"

namespace truegraphics::core {

namespace {
thread_local App* g_current_app = nullptr;
}

App::App() {
  windows_.push_back(WindowState{});
  windows_[0].root = std::make_shared<widgets::Container>();

  style_engine_.set_theme(windows_[0].config.dark_theme ? style::Theme::dark() : style::Theme::light());
  windows_[0].root->style() = windows_[0].config.dark_theme ? style::Theme::dark() : style::Theme::light();
  windows_[0].root->set_bounds(0, 0, windows_[0].config.width, windows_[0].config.height);
}

App::~App() {
  async_.wait_all();
  set_current_app(nullptr);
}

void App::set_builder(Builder builder) { windows_[0].builder = std::move(builder); }

int App::add_window(const std::string& title, int width, int height, Builder builder) {
  WindowState w;
  w.config.title = title;
  w.config.width = width;
  w.config.height = height;
  w.config.dark_theme = windows_[0].config.dark_theme;
  w.builder = std::move(builder);
  w.root = std::make_shared<widgets::Container>();
  w.root->style() = w.config.dark_theme ? style::Theme::dark() : style::Theme::light();
  w.root->set_bounds(0, 0, w.config.width, w.config.height);
  windows_.push_back(std::move(w));
  return static_cast<int>(windows_.size() - 1);
}

App::WindowState& App::window_state(size_t index) {
  if (index >= windows_.size()) {
    return windows_[0];
  }
  return windows_[index];
}

const App::WindowState& App::window_state(size_t index) const {
  if (index >= windows_.size()) {
    return windows_[0];
  }
  return windows_[index];
}

void App::build_ui() {
  build_window_ui(0);
}

void App::build_all_ui() {
  begin_build();
  for (size_t i = 0; i < windows_.size(); ++i) {
    build_window_ui(i);
  }
  end_build();
}

void App::build_window_ui(size_t index) {
  building_window_index_ = index;
  auto& ws = window_state(index);

  ws.root = std::make_shared<widgets::Container>();
  ws.root->style() = ws.config.dark_theme ? style::Theme::dark() : style::Theme::light();
  ws.root->set_bounds(0, 0, ws.config.width, ws.config.height);

  build_root_ = ws.root;
  if (ws.builder) {
    ws.builder();
  }
  layout_.apply(ws.root);
  build_root_.reset();
}

int App::run() {
  build_all_ui();
  Engine engine;
  return engine.run(*this);
}

void App::set_title(const std::string& title) { windows_[0].config.title = title; }

void App::set_size(int width, int height) {
  windows_[0].config.width = width;
  windows_[0].config.height = height;
  if (windows_[0].root) {
    windows_[0].root->set_size(width, height);
  }
}

void App::set_theme(bool dark) {
  windows_[0].config.dark_theme = dark;
  style_engine_.set_theme(dark ? style::Theme::dark() : style::Theme::light());
  for (auto& w : windows_) {
    w.config.dark_theme = dark;
    if (w.root) {
      w.root->style() = dark ? style::Theme::dark() : style::Theme::light();
    }
  }
}

void App::set_fullscreen(bool fullscreen) { windows_[0].config.fullscreen = fullscreen; }

void App::set_icon(const std::string& icon_path) { windows_[0].config.icon_path = resource::resolve(icon_path); }

void App::set_min_size(int32_t w, int32_t h) {
  windows_[0].config.min_width = w;
  windows_[0].config.min_height = h;
}

void App::set_max_size(int32_t w, int32_t h) {
  windows_[0].config.max_width = w;
  windows_[0].config.max_height = h;
}

Config& App::config() { return windows_[0].config; }
const Config& App::config() const { return windows_[0].config; }

size_t App::window_count() const { return windows_.size(); }
const Config& App::window_config(size_t index) const { return window_state(index).config; }
std::shared_ptr<widgets::Container> App::window_root(size_t index) { return window_state(index).root; }

void App::begin_build() {
  building_ = true;
  set_current_app(this);
}

void App::end_build() { building_ = false; }

std::shared_ptr<widgets::Container> App::root() { return windows_[0].root; }

std::shared_ptr<widgets::Widget> App::attach_widget(
    const std::shared_ptr<widgets::Widget>& widget,
    const std::shared_ptr<widgets::Widget>& parent) {
  if (windows_.empty()) {
    windows_.push_back(WindowState{});
    windows_[0].root = std::make_shared<widgets::Container>();
  }
  style_engine_.apply_defaults(widget);
  if (parent) {
    parent->add_child(widget);
  } else {
    if (build_root_) {
      build_root_->add_child(widget);
    } else {
      windows_[0].root->add_child(widget);
    }
  }
  return widget;
}

void App::render(graphics::Renderer& renderer) {
  render(renderer, 0);
}

void App::render(graphics::Renderer& renderer, size_t window_index) {
  auto root = window_state(window_index).root;
  if (root) {
    root->draw(renderer);
  }
}

namespace {
void apply_animators_to_widget(const std::shared_ptr<widgets::Widget>& w, animation::Animator& animator) {
  if (!w) return;

  const std::string prefix = w->id() + ":";

  int x = w->x();
  int y = w->y();
  int width = w->width();
  int height = w->height();

  const std::string kx = prefix + "x";
  const std::string ky = prefix + "y";
  const std::string kw = prefix + "width";
  const std::string kh = prefix + "height";

  if (animator.active(kx)) x = static_cast<int>(animator.value(kx, static_cast<double>(x)));
  if (animator.active(ky)) y = static_cast<int>(animator.value(ky, static_cast<double>(y)));
  if (animator.active(kw)) width = static_cast<int>(animator.value(kw, static_cast<double>(width)));
  if (animator.active(kh)) height = static_cast<int>(animator.value(kh, static_cast<double>(height)));

  w->set_bounds(x, y, width, height);

  for (const auto& child : w->children()) {
    apply_animators_to_widget(child, animator);
  }
}
}

void App::tick(double dt) {
  animator_.update(dt);

  // main-thread posts
  {
    std::vector<std::function<void()>> to_run;
    {
      std::lock_guard<std::mutex> lock(post_mutex_);
      to_run.swap(post_queue_);
    }
    for (auto& fn : to_run) {
      if (fn) fn();
    }
  }

  for (auto& ws : windows_) {
    if (ws.root) {
      apply_animators_to_widget(ws.root, animator_);
    }
  }

  // timers
  if (!timers_.empty()) {
    std::vector<std::function<void()>> to_run;

    for (auto it = timers_.begin(); it != timers_.end();) {
      it->remaining -= dt;
      if (it->remaining <= 0.0) {
        if (it->cb) {
          to_run.push_back(it->cb);
        }
        if (it->repeat) {
          it->remaining += it->interval;
          ++it;
        } else {
          it = timers_.erase(it);
        }
      } else {
        ++it;
      }
    }

    for (auto& cb : to_run) {
      cb();
    }
  }

  animator_.cleanup_finished();
}

void App::click(int x, int y) {
  click(x, y, 0);
}

void App::on_mouse_left(int x, int y, bool pressed) {
  on_mouse_left(x, y, pressed, 0);
}

void App::on_resize(int width, int height) {
  on_resize(width, height, 0);
}

void App::click(int x, int y, size_t window_index) {
  auto& ws = window_state(window_index);
  if (!ws.root) return;

  if (auto hit = ws.root->hit_test_deep(x, y)) {
    hit->click();
    if (hit->focusable()) {
      if (auto prev = ws.focused.lock()) {
        prev->set_focused(false);
      }
      ws.focused = hit;
      hit->set_focused(true);
    }
  }
}

void App::on_mouse_left(int x, int y, bool pressed, size_t window_index) {
  auto& ws = window_state(window_index);
  ws.left_down = pressed;

  if (pressed) {
    auto hit = ws.root ? ws.root->hit_test_deep(x, y) : nullptr;
    if (hit) {
      hit->mouse_down(x, y);
      // Preserve current click semantics (press triggers click).
      hit->click();

      if (hit->focusable()) {
        if (auto prev = ws.focused.lock()) {
          prev->set_focused(false);
        }
        ws.focused = hit;
        hit->set_focused(true);
      }

      ws.dragging = hit;
    } else {
      ws.dragging.reset();
    }
  } else {
    if (auto d = ws.dragging.lock()) {
      d->mouse_up(x, y);
    }
    ws.dragging.reset();
  }
}

void App::on_resize(int width, int height, size_t window_index) {
  auto& ws = window_state(window_index);
  ws.config.width = width;
  ws.config.height = height;
  if (ws.root) {
    ws.root->set_bounds(0, 0, width, height);
    layout_.apply(ws.root);
  }
}

void App::animate_widget(
    const std::shared_ptr<widgets::Widget>& widget,
    const std::string& property,
    double to,
    double duration,
    bool smoothstep) {
  if (!widget) return;

  double start = 0.0;
  if (property == "x") start = widget->x();
  else if (property == "y") start = widget->y();
  else if (property == "width") start = widget->width();
  else if (property == "height") start = widget->height();
  else return;

  animator_.animate(widget->id() + ":" + property, start, to, duration, smoothstep);
}

int App::set_timeout(double seconds, std::function<void()> cb) {
  const int id = next_timer_id_++;
  timers_.push_back(Timer{id, seconds, seconds, false, std::move(cb)});
  return id;
}

int App::set_interval(double seconds, std::function<void()> cb) {
  const int id = next_timer_id_++;
  timers_.push_back(Timer{id, seconds, seconds, true, std::move(cb)});
  return id;
}

void App::clear_timer(int id) {
  timers_.erase(
      std::remove_if(timers_.begin(), timers_.end(), [id](const Timer& t) { return t.id == id; }),
      timers_.end());
}

void App::post(std::function<void()> fn) {
  std::lock_guard<std::mutex> lock(post_mutex_);
  post_queue_.push_back(std::move(fn));
}

void App::run_async(std::function<void()> task, std::function<void()> on_done) {
  async_.schedule([this, task = std::move(task), on_done = std::move(on_done)]() mutable {
    if (task) {
      task();
    }
    if (on_done) {
      post(std::move(on_done));
    }
  });
}

void App::on_mouse_move(int x, int y) {
  on_mouse_move(x, y, 0);
}

void App::on_mouse_wheel(int x, int y, int delta) {
  on_mouse_wheel(x, y, delta, 0);
}

void App::on_key_down(int32_t key) {
  on_key_down(key, 0);
}

void App::on_char(uint32_t codepoint) {
  on_char(codepoint, 0);
}

void App::on_shortcut(std::string combo, std::function<void()> cb) { shortcuts_[std::move(combo)] = std::move(cb); }

void App::on_drop_files(int x, int y, std::vector<std::string> files) {
  on_drop_files(x, y, std::move(files), 0);
}

void App::on_mouse_move(int x, int y, size_t window_index) {
  auto& ws = window_state(window_index);
  if (!ws.root) return;

  auto hit = ws.root->hit_test_deep(x, y);

  if (ws.left_down) {
    if (auto d = ws.dragging.lock()) {
      d->mouse_move(x, y);
    }
  }

  auto prev = ws.hovered.lock();
  if (prev && prev.get() != hit.get()) {
    prev->set_hovered(false);
  }

  if (hit) {
    hit->set_hovered(true);
    hit->mouse_move(x, y);
  }

  ws.hovered = hit;
}

void App::on_mouse_wheel(int x, int y, int delta, size_t window_index) {
  auto& ws = window_state(window_index);
  if (!ws.root) return;

  auto hit = ws.root->hit_test_deep(x, y);
  auto current = hit;
  const bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

  while (current) {
    if (auto scroll = std::dynamic_pointer_cast<widgets::ScrollView>(current)) {
      const int lines = delta / 120;
      if (shift) {
        scroll->scroll_by_x(-lines * 36);
      } else {
        scroll->scroll_by(-lines * 36);
      }
      return;
    }
    current = current->parent();
  }
}

void App::on_key_down(int32_t key, size_t window_index) {
  // Shortcut dispatch (Ctrl/Shift/Alt + key)
  const bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
  const bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
  const bool alt = (GetKeyState(VK_MENU) & 0x8000) != 0;

  std::string combo;
  if (ctrl) combo += "Ctrl+";
  if (shift) combo += "Shift+";
  if (alt) combo += "Alt+";

  if (key >= 'A' && key <= 'Z') {
    combo += static_cast<char>(key);
  } else if (key >= VK_F1 && key <= VK_F24) {
    combo += "F" + std::to_string((key - VK_F1) + 1);
  } else if (key == VK_ESCAPE) {
    combo += "Esc";
  } else if (key == VK_RETURN) {
    combo += "Enter";
  } else if (key == VK_TAB) {
    combo += "Tab";
  }

  if (!combo.empty()) {
    if (auto it = shortcuts_.find(combo); it != shortcuts_.end()) {
      if (it->second) {
        it->second();
      }
    }
  }

  auto& ws = window_state(window_index);

  // Focus traversal (Tab / Shift+Tab).
  if (key == VK_TAB && !ctrl && !alt) {
    std::vector<std::shared_ptr<widgets::Widget>> focusables;
    focusables.reserve(64);

    std::function<void(const std::shared_ptr<widgets::Widget>&)> walk;
    walk = [&](const std::shared_ptr<widgets::Widget>& w) {
      if (!w) return;
      if (w->focusable()) {
        focusables.push_back(w);
      }
      for (const auto& child : w->children()) {
        walk(child);
      }
    };
    walk(ws.root);

    if (!focusables.empty()) {
      auto current = ws.focused.lock();
      int idx = -1;
      for (int i = 0; i < static_cast<int>(focusables.size()); ++i) {
        if (focusables[static_cast<size_t>(i)].get() == current.get()) {
          idx = i;
          break;
        }
      }

      int next = 0;
      if (idx >= 0) {
        next = shift ? (idx - 1) : (idx + 1);
        if (next < 0) next = static_cast<int>(focusables.size() - 1);
        if (next >= static_cast<int>(focusables.size())) next = 0;
      }

      if (current) {
        current->set_focused(false);
      }
      ws.focused = focusables[static_cast<size_t>(next)];
      focusables[static_cast<size_t>(next)]->set_focused(true);
    }
    return;
  }

  if (auto f = ws.focused.lock()) {
    f->key_down(key);
  }
}

void App::on_char(uint32_t codepoint, size_t window_index) {
  auto& ws = window_state(window_index);
  if (auto f = ws.focused.lock()) {
    f->char_input(codepoint);
  }
}

void App::on_drop_files(int x, int y, std::vector<std::string> files, size_t window_index) {
  auto& ws = window_state(window_index);
  if (!ws.root) return;
  if (auto hit = ws.root->hit_test_deep(x, y)) {
    hit->drop_files(std::move(files));
  }
}

void App::enable_tray(const std::string& icon_path, const std::string& tooltip, std::function<void()> on_click) {
  tray_.enabled = true;
  tray_.icon_path = resource::resolve(icon_path);
  tray_.tooltip = tooltip;
  tray_.on_click = std::move(on_click);
}

void App::disable_tray() {
  tray_ = TrayState{};
}

bool App::tray_enabled() const { return tray_.enabled; }
const std::string& App::tray_icon_path() const { return tray_.icon_path; }
const std::string& App::tray_tooltip() const { return tray_.tooltip; }
void App::tray_click() {
  if (tray_.on_click) {
    tray_.on_click();
  }
}

App* current_app() { return g_current_app; }
void set_current_app(App* app) { g_current_app = app; }

}  // namespace truegraphics::core

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "truegraphics/animation/Animator.h"
#include "truegraphics/async/Async.h"
#include "truegraphics/core/Config.h"
#include "truegraphics/layout/FlexLayout.h"
#include "truegraphics/plugin/Plugin.h"
#include "truegraphics/state/State.h"
#include "truegraphics/style/StyleEngine.h"
#include "truegraphics/system/Profiler.h"

namespace truegraphics::widgets {
class Widget;
class Container;
}

namespace truegraphics::graphics {
class Renderer;
}

namespace truegraphics::core {

class App final {
 public:
  using Builder = std::function<void()>;

  App();
  ~App();

  void set_builder(Builder builder);
  // Registers a secondary window that will be created when `run()` is called.
  // Returns the window index (0 = main window).
  int add_window(const std::string& title, int width, int height, Builder builder);
  void build_ui();
  void build_all_ui();
  int run();

  void set_title(const std::string& title);
  void set_size(int width, int height);
  void set_theme(bool dark);
  void set_fullscreen(bool fullscreen);
  void set_icon(const std::string& icon_path);
  void set_min_size(int32_t w, int32_t h);
  void set_max_size(int32_t w, int32_t h);
  Config& config();
  const Config& config() const;

  size_t window_count() const;
  const Config& window_config(size_t index) const;
  std::shared_ptr<widgets::Container> window_root(size_t index);

  void begin_build();
  void end_build();

  std::shared_ptr<widgets::Container> root();
  std::shared_ptr<widgets::Widget> attach_widget(
      const std::shared_ptr<widgets::Widget>& widget,
      const std::shared_ptr<widgets::Widget>& parent = nullptr);

  void render(graphics::Renderer& renderer);
  void render(graphics::Renderer& renderer, size_t window_index);
  void tick(double dt);
  void click(int x, int y);
  void on_resize(int width, int height);
  void on_mouse_move(int x, int y);
  void on_mouse_left(int x, int y, bool pressed);
  void on_mouse_wheel(int x, int y, int delta);
  void on_key_down(int32_t key);
  void on_char(uint32_t codepoint);
  void on_shortcut(std::string combo, std::function<void()> cb);
  void on_drop_files(int x, int y, std::vector<std::string> files);

  void click(int x, int y, size_t window_index);
  void on_resize(int width, int height, size_t window_index);
  void on_mouse_move(int x, int y, size_t window_index);
  void on_mouse_left(int x, int y, bool pressed, size_t window_index);
  void on_mouse_wheel(int x, int y, int delta, size_t window_index);
  void on_key_down(int32_t key, size_t window_index);
  void on_char(uint32_t codepoint, size_t window_index);
  void on_drop_files(int x, int y, std::vector<std::string> files, size_t window_index);

  void animate_widget(const std::shared_ptr<widgets::Widget>& widget, const std::string& property, double to, double duration,
                      bool smoothstep = true);

  int set_timeout(double seconds, std::function<void()> cb);
  int set_interval(double seconds, std::function<void()> cb);
  void clear_timer(int id);

  void post(std::function<void()> fn);
  void run_async(std::function<void()> task, std::function<void()> on_done = nullptr);

  void enable_tray(const std::string& icon_path, const std::string& tooltip, std::function<void()> on_click = nullptr);
  void disable_tray();
  bool tray_enabled() const;
  const std::string& tray_icon_path() const;
  const std::string& tray_tooltip() const;
  void tray_click();

 private:
  struct WindowState {
    Config config;
    Builder builder;
    std::shared_ptr<widgets::Container> root;
    std::weak_ptr<widgets::Widget> hovered;
    std::weak_ptr<widgets::Widget> focused;
    std::weak_ptr<widgets::Widget> dragging;
    bool left_down = false;
  };

  WindowState& window_state(size_t index);
  const WindowState& window_state(size_t index) const;
  void build_window_ui(size_t index);

  std::vector<WindowState> windows_;
  size_t building_window_index_ = 0;
  std::shared_ptr<widgets::Container> build_root_;

  layout::FlexLayout layout_;
  style::StyleEngine style_engine_;
  animation::Animator animator_;
  async::AsyncExecutor async_;
  plugin::PluginManager plugins_;
  system::Profiler profiler_;
  bool building_ = false;

  struct TrayState {
    bool enabled = false;
    std::string icon_path;
    std::string tooltip;
    std::function<void()> on_click;
  } tray_;

  struct Timer {
    int id = 0;
    double interval = 0.0;
    double remaining = 0.0;
    bool repeat = false;
    std::function<void()> cb;
  };

  int next_timer_id_ = 1;
  std::vector<Timer> timers_;

  std::mutex post_mutex_;
  std::vector<std::function<void()>> post_queue_;

  std::unordered_map<std::string, std::function<void()>> shortcuts_;
};

App* current_app();
void set_current_app(App* app);

}  // namespace truegraphics::core

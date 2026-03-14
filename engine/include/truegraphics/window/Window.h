#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <windows.h>

namespace truegraphics::core {
struct Config;
}

namespace truegraphics::window {

class Window final {
 public:
  using TickFn = std::function<void(void* paint_dc)>;
  using ResizeFn = std::function<void(int32_t, int32_t)>;
  using MouseMoveFn = std::function<void(int32_t, int32_t)>;
  using MouseBtnFn = std::function<void(int32_t, int32_t, bool)>;
  using MouseWheelFn = std::function<void(int32_t, int32_t, int32_t)>;
  using KeyFn = std::function<void(int32_t)>;
  using CharFn = std::function<void(uint32_t)>;
  using DropFilesFn = std::function<void(int32_t, int32_t, std::vector<std::string>)>;

  Window();
  ~Window();

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  bool create(const core::Config& config);
  void show();
  void close();
  void invalidate();
  int run_loop();

  void set_tick(TickFn fn);
  void set_resize(ResizeFn fn);
  void set_mouse_move(MouseMoveFn fn);
  void set_mouse_left(MouseBtnFn fn);
  void set_mouse_wheel(MouseWheelFn fn);
  void set_key_down(KeyFn fn);
  void set_char(CharFn fn);
  void set_drop_files(DropFilesFn fn);

  void enable_tray_icon(const std::string& icon_path, const std::string& tooltip, std::function<void()> on_click = nullptr);
  void disable_tray_icon();

  void* native_handle() const;
  int32_t width() const;
  int32_t height() const;
  bool should_close() const;

 private:
  static intptr_t wndproc_thunk(void* hwnd, uint32_t msg, uintptr_t wparam, intptr_t lparam);
  intptr_t wndproc(void* hwnd, uint32_t msg, uintptr_t wparam, intptr_t lparam);

  void* hwnd_ = nullptr;
  int32_t width_ = 0;
  int32_t height_ = 0;
  bool should_close_ = false;
  uintptr_t frame_timer_id_ = 0;

  int32_t min_width_ = 0;
  int32_t min_height_ = 0;
  int32_t max_width_ = 0;
  int32_t max_height_ = 0;
  bool fullscreen_ = false;
  uintptr_t windowed_style_ = 0;
  RECT windowed_rect_{};

  struct TrayState;
  std::unique_ptr<TrayState> tray_;

  TickFn tick_;
  ResizeFn resize_;
  MouseMoveFn mouse_move_;
  MouseBtnFn mouse_left_;
  MouseWheelFn mouse_wheel_;
  KeyFn key_down_;
  CharFn char_;
  DropFilesFn drop_files_;
};

}  // namespace truegraphics::window

#include "truegraphics/window/Window.h"

#include "truegraphics/core/Config.h"

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>

#include <string>

namespace truegraphics::window {

namespace {
std::wstring to_wide(const std::string& input) {
  return std::wstring(input.begin(), input.end());
}
}

Window::Window() = default;
Window::~Window() { close(); }

namespace {
long g_window_count = 0;
}

namespace {
void set_window_icon(HWND hwnd, const std::string& icon_path) {
  if (!hwnd || icon_path.empty()) return;

  auto wide = to_wide(icon_path);
  HICON icon = static_cast<HICON>(LoadImageW(nullptr, wide.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
  if (!icon) return;

  SendMessageW(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
  SendMessageW(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
}

void enable_fullscreen(HWND hwnd, bool& fullscreen, uintptr_t& windowed_style, RECT& windowed_rect) {
  if (!hwnd || fullscreen) return;

  fullscreen = true;
  windowed_style = static_cast<uintptr_t>(GetWindowLongPtrW(hwnd, GWL_STYLE));
  GetWindowRect(hwnd, &windowed_rect);

  MONITORINFO mi{};
  mi.cbSize = sizeof(mi);
  GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi);

  SetWindowLongPtrW(hwnd, GWL_STYLE, static_cast<LONG_PTR>(windowed_style & ~WS_OVERLAPPEDWINDOW));
  SetWindowPos(
      hwnd,
      HWND_TOP,
      mi.rcMonitor.left,
      mi.rcMonitor.top,
      mi.rcMonitor.right - mi.rcMonitor.left,
      mi.rcMonitor.bottom - mi.rcMonitor.top,
      SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}
}

bool Window::create(const core::Config& config) {
  HINSTANCE instance = GetModuleHandleW(nullptr);
  static const wchar_t* kClassName = L"TrueGraphicsWindowClass";

  WNDCLASSW wc{};
  wc.lpfnWndProc = reinterpret_cast<WNDPROC>(&Window::wndproc_thunk);
  wc.hInstance = instance;
  wc.lpszClassName = kClassName;
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

  // IMPORTANT: disable default background erase
  wc.hbrBackground = nullptr;

  RegisterClassW(&wc);

  DWORD style = WS_OVERLAPPEDWINDOW;
  if (!config.resizable) {
    style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
  }

  RECT rect{0, 0, static_cast<LONG>(config.width), static_cast<LONG>(config.height)};
  AdjustWindowRect(&rect, style, FALSE);

  HWND hwnd = CreateWindowExW(
      0, kClassName, to_wide(config.title).c_str(), style,
      CW_USEDEFAULT, CW_USEDEFAULT,
      rect.right - rect.left,
      rect.bottom - rect.top,
      nullptr, nullptr, instance, this);

  hwnd_ = hwnd;
  width_ = config.width;
  height_ = config.height;
  should_close_ = hwnd == nullptr;

  min_width_ = config.min_width;
  min_height_ = config.min_height;
  max_width_ = config.max_width;
  max_height_ = config.max_height;

  if (hwnd) {
    set_window_icon(hwnd, config.icon_path);
    if (config.fullscreen) {
      enable_fullscreen(hwnd, fullscreen_, windowed_style_, windowed_rect_);
    }
  }

  if (hwnd) {
    // Drive continuous redraw through WM_TIMER -> InvalidateRect -> WM_PAINT.
    frame_timer_id_ = static_cast<uintptr_t>(SetTimer(hwnd, 1, 16, nullptr));
    InterlockedIncrement(&g_window_count);
    DragAcceptFiles(hwnd, TRUE);
  }

  return hwnd != nullptr;
}

void Window::show() {
  if (hwnd_) {
    ShowWindow(static_cast<HWND>(hwnd_), SW_SHOW);
    UpdateWindow(static_cast<HWND>(hwnd_));
  }
}

void Window::close() {
  if (hwnd_) {
    if (frame_timer_id_) {
      KillTimer(static_cast<HWND>(hwnd_), static_cast<UINT_PTR>(frame_timer_id_));
      frame_timer_id_ = 0;
    }
    DestroyWindow(static_cast<HWND>(hwnd_));
    hwnd_ = nullptr;
  }
  should_close_ = true;
}

void Window::invalidate() {
  if (hwnd_) {
    InvalidateRect(static_cast<HWND>(hwnd_), nullptr, FALSE);
  }
}

int Window::run_loop() {
  MSG msg{};

  while (!should_close_) {
    int res = GetMessageW(&msg, nullptr, 0, 0);
    if (res == 0) {
      should_close_ = true;
      return static_cast<int>(msg.wParam);
    }
    if (res == -1) {
      should_close_ = true;
      return -1;
    }

    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  return static_cast<int>(msg.wParam);
}

void Window::set_tick(TickFn fn) { tick_ = std::move(fn); }
void Window::set_resize(ResizeFn fn) { resize_ = std::move(fn); }
void Window::set_mouse_move(MouseMoveFn fn) { mouse_move_ = std::move(fn); }
void Window::set_mouse_left(MouseBtnFn fn) { mouse_left_ = std::move(fn); }
void Window::set_mouse_wheel(MouseWheelFn fn) { mouse_wheel_ = std::move(fn); }
void Window::set_key_down(KeyFn fn) { key_down_ = std::move(fn); }
void Window::set_char(CharFn fn) { char_ = std::move(fn); }
void Window::set_drop_files(DropFilesFn fn) { drop_files_ = std::move(fn); }

struct Window::TrayState {
  NOTIFYICONDATAW nid{};
  HICON icon = nullptr;
  bool own_icon = false;
  std::function<void()> on_click;
  HMENU menu = nullptr;
  UINT callback_msg = WM_APP + 42;
  bool added = false;
};

void Window::enable_tray_icon(const std::string& icon_path, const std::string& tooltip, std::function<void()> on_click) {
  HWND hwnd = static_cast<HWND>(hwnd_);
  if (!hwnd) return;

  if (!tray_) {
    tray_ = std::make_unique<TrayState>();
  }

  tray_->on_click = std::move(on_click);

  if (tray_->added) {
    disable_tray_icon();
    if (!tray_) tray_ = std::make_unique<TrayState>();
  }

  std::wstring wide_icon = to_wide(icon_path);
  HICON icon = nullptr;
  bool own_icon = false;
  if (!wide_icon.empty()) {
    icon = static_cast<HICON>(LoadImageW(nullptr, wide_icon.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
    own_icon = icon != nullptr;
  }
  if (!icon) {
    // 32512 == IDI_APPLICATION
    icon = static_cast<HICON>(LoadIconW(nullptr, MAKEINTRESOURCEW(32512)));
  }

  tray_->icon = icon;
  tray_->own_icon = own_icon;
  tray_->nid = NOTIFYICONDATAW{};
  tray_->nid.cbSize = sizeof(NOTIFYICONDATAW);
  tray_->nid.hWnd = hwnd;
  tray_->nid.uID = 1;
  tray_->nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
  tray_->nid.uCallbackMessage = tray_->callback_msg;
  tray_->nid.hIcon = icon;

  std::wstring wide_tip = to_wide(tooltip);
  if (wide_tip.empty()) wide_tip = L"TrueGraphics";
  wcsncpy_s(tray_->nid.szTip, wide_tip.c_str(), _TRUNCATE);

  tray_->added = Shell_NotifyIconW(NIM_ADD, &tray_->nid) != FALSE;
  if (tray_->added) {
    tray_->nid.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &tray_->nid);
  }
}

void Window::disable_tray_icon() {
  HWND hwnd = static_cast<HWND>(hwnd_);
  if (!hwnd) return;
  if (!tray_) return;

  if (tray_->added) {
    Shell_NotifyIconW(NIM_DELETE, &tray_->nid);
    tray_->added = false;
  }
  if (tray_->menu) {
    DestroyMenu(tray_->menu);
    tray_->menu = nullptr;
  }
  if (tray_->icon && tray_->own_icon) {
    DestroyIcon(tray_->icon);
    tray_->icon = nullptr;
  }
}

void* Window::native_handle() const { return hwnd_; }
int32_t Window::width() const { return width_; }
int32_t Window::height() const { return height_; }
bool Window::should_close() const { return should_close_; }

intptr_t Window::wndproc_thunk(void* hwnd, uint32_t msg, uintptr_t wparam, intptr_t lparam) {

  HWND handle = static_cast<HWND>(hwnd);

  if (msg == WM_NCCREATE) {
    auto* create = reinterpret_cast<CREATESTRUCTW*>(lparam);
    auto* self = static_cast<Window*>(create->lpCreateParams);

    SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));

    return self->wndproc(hwnd, msg, wparam, lparam);
  }

  auto* self = reinterpret_cast<Window*>(GetWindowLongPtrW(handle, GWLP_USERDATA));

  if (self) {
    return self->wndproc(hwnd, msg, wparam, lparam);
  }

  return DefWindowProcW(handle, msg, wparam, lparam);
}

intptr_t Window::wndproc(void* hwnd, uint32_t msg, uintptr_t wparam, intptr_t lparam) {

  HWND handle = static_cast<HWND>(hwnd);

  switch (msg) {

    // IMPORTANT: stop Windows background clearing
    case WM_ERASEBKGND:
      return 1;

    case WM_TIMER:
      if (tick_) {
        InvalidateRect(handle, nullptr, FALSE);
      }
      return 0;

    case WM_PAINT: {
      PAINTSTRUCT ps{};
      HDC paint_dc = BeginPaint(handle, &ps);
      if (tick_) {
        tick_(paint_dc);
      }
      EndPaint(handle, &ps);
      return 0;
    }

    case WM_GETMINMAXINFO: {
      auto* info = reinterpret_cast<MINMAXINFO*>(lparam);
      if (min_width_ > 0) info->ptMinTrackSize.x = min_width_;
      if (min_height_ > 0) info->ptMinTrackSize.y = min_height_;
      if (max_width_ > 0) info->ptMaxTrackSize.x = max_width_;
      if (max_height_ > 0) info->ptMaxTrackSize.y = max_height_;
      return 0;
    }

    case WM_SIZE: {
      width_ = LOWORD(lparam);
      height_ = HIWORD(lparam);

      if (resize_) {
        resize_(width_, height_);
      }

      InvalidateRect(handle, nullptr, FALSE);
      return 0;
    }

    case WM_MOUSEMOVE:
      if (mouse_move_) {
        mouse_move_(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
      }
      return 0;

    case WM_MOUSEWHEEL: {
      if (mouse_wheel_) {
        const int delta = GET_WHEEL_DELTA_WPARAM(wparam);
        POINT pt{GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)};  // screen coords
        ScreenToClient(handle, &pt);
        mouse_wheel_(pt.x, pt.y, delta);
      }
      return 0;
    }

    case WM_KEYDOWN:
      if (key_down_) {
        key_down_(static_cast<int32_t>(wparam));
      }
      return 0;

    case WM_CHAR:
      if (char_) {
        char_(static_cast<uint32_t>(wparam));
      }
      return 0;

    case WM_DROPFILES: {
      if (!drop_files_) {
        DragFinish(reinterpret_cast<HDROP>(wparam));
        return 0;
      }

      HDROP drop = reinterpret_cast<HDROP>(wparam);
      POINT pt{};
      DragQueryPoint(drop, &pt);

      UINT count = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
      std::vector<std::string> files;
      files.reserve(count);

      wchar_t buffer[MAX_PATH];
      for (UINT i = 0; i < count; ++i) {
        UINT len = DragQueryFileW(drop, i, buffer, MAX_PATH);
        if (len > 0) {
          std::wstring ws(buffer, buffer + len);
          files.emplace_back(ws.begin(), ws.end());
        }
      }

      DragFinish(drop);
      drop_files_(pt.x, pt.y, std::move(files));
      return 0;
    }

    case WM_LBUTTONDOWN:
      if (mouse_left_) {
        mouse_left_(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), true);
      }
      return 0;

    case WM_LBUTTONUP:
      if (mouse_left_) {
        mouse_left_(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), false);
      }
      return 0;

    case WM_CLOSE:
      should_close_ = true;
      DestroyWindow(handle);
      return 0;

    case WM_DESTROY:
      should_close_ = true;
      if (frame_timer_id_) {
        KillTimer(handle, static_cast<UINT_PTR>(frame_timer_id_));
        frame_timer_id_ = 0;
      }
      disable_tray_icon();
      if (InterlockedDecrement(&g_window_count) <= 0) {
        PostQuitMessage(0);
      }
      return 0;

    default:
      if (tray_ && msg == tray_->callback_msg) {
        if (lparam == WM_LBUTTONUP) {
          if (tray_->on_click) tray_->on_click();
          return 0;
        }
        if (lparam == WM_RBUTTONUP) {
          POINT pt{};
          GetCursorPos(&pt);
          SetForegroundWindow(handle);

          if (!tray_->menu) {
            tray_->menu = CreatePopupMenu();
            InsertMenuW(tray_->menu, 0, MF_BYPOSITION | MF_STRING, 1, L"Exit");
          }

          const UINT cmd = TrackPopupMenu(tray_->menu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, handle, nullptr);
          if (cmd == 1) {
            PostQuitMessage(0);
          }
          return 0;
        }
      }
      return DefWindowProcW(handle, msg, wparam, lparam);
  }
}

}

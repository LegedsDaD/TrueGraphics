#include "truegraphics/graphics/Renderer.h"
#include "truegraphics/window/Window.h"

#include "truegraphics/graphics/Image.h"

#include <windows.h>
#include <windowsx.h>

#include <memory>

#include "truegraphics/widgets/Widget.h"

#include <objidl.h>
#include <gdiplus.h>

#include <mutex>
#include <unordered_map>

namespace truegraphics::graphics {

namespace {
COLORREF to_colorref(Color color) {
  return RGB(color.r, color.g, color.b);
}
}

namespace {
HFONT get_font_cached(HDC hdc, const std::string& family, int size) {
  static std::unordered_map<std::string, HFONT> cache;
  static std::mutex m;

  if (!hdc) return nullptr;
  if (size <= 0) size = 16;

  const std::string key = family + ":" + std::to_string(size);
  {
    std::lock_guard<std::mutex> lock(m);
    if (auto it = cache.find(key); it != cache.end()) {
      return it->second;
    }

    const int height = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    std::wstring wfamily(family.begin(), family.end());
    HFONT font = CreateFontW(height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, wfamily.c_str());
    cache[key] = font;
    return font;
  }
}
}

Renderer::Renderer() = default;
Renderer::~Renderer() {
  if (gdiplus_token_) {
    Gdiplus::GdiplusShutdown(static_cast<ULONG_PTR>(gdiplus_token_));
    gdiplus_token_ = 0;
  }

  if (mem_dc_) {
    if (mem_old_bitmap_) {
      SelectObject(mem_dc_, mem_old_bitmap_);
    }
    if (mem_bitmap_) {
      DeleteObject(mem_bitmap_);
    }
    DeleteDC(mem_dc_);
  }

  mem_dc_ = nullptr;
  mem_bitmap_ = nullptr;
  mem_old_bitmap_ = nullptr;
  present_dc_ = nullptr;
  hdc_ = nullptr;
}

bool Renderer::initialize(window::Window* window) {
  hwnd_ = window ? static_cast<HWND>(window->native_handle()) : nullptr;

  if (!gdiplus_token_) {
    Gdiplus::GdiplusStartupInput input{};
    ULONG_PTR token = 0;
    if (Gdiplus::GdiplusStartup(&token, &input, nullptr) == Gdiplus::Ok) {
      gdiplus_token_ = static_cast<uintptr_t>(token);
    }
  }

  return hwnd_ != nullptr;
}

void Renderer::begin_frame(void* present_dc) {
  HWND hwnd = static_cast<HWND>(hwnd_);
  if (!hwnd) return;

  RECT rect{};
  GetClientRect(hwnd, &rect);

  int w = rect.right - rect.left;
  int h = rect.bottom - rect.top;
  if (w <= 0 || h <= 0) return;

  present_dc_ = static_cast<HDC>(present_dc);

  HDC window_dc = GetDC(hwnd);

  if (!mem_dc_) {
    mem_dc_ = CreateCompatibleDC(window_dc);
  }

  if (mem_dc_ && (!mem_bitmap_ || width_ != w || height_ != h)) {
    HBITMAP new_bitmap = CreateCompatibleBitmap(window_dc, w, h);
    if (new_bitmap) {
      HGDIOBJ previous = SelectObject(mem_dc_, new_bitmap);
      if (!mem_old_bitmap_) {
        mem_old_bitmap_ = previous;
      } else if (mem_bitmap_) {
        // `previous` is the currently selected backbuffer bitmap.
      }
      if (mem_bitmap_) {
        DeleteObject(mem_bitmap_);
      }
      mem_bitmap_ = new_bitmap;
    }
  }

  width_ = w;
  height_ = h;

  ReleaseDC(hwnd, window_dc);

  hdc_ = mem_dc_;
}

void Renderer::end_frame() {

  HWND hwnd = static_cast<HWND>(hwnd_);
  if (!hwnd) return;
  if (!mem_dc_) return;

  HDC dst_dc = present_dc_;
  bool release_dst = false;

  if (!dst_dc) {
    dst_dc = GetDC(hwnd);
    release_dst = true;
  }

  BitBlt(
      dst_dc,
      0, 0,
      width_, height_,
      mem_dc_,
      0, 0,
      SRCCOPY);

  if (release_dst) {
    ReleaseDC(hwnd, dst_dc);
  }

  present_dc_ = nullptr;
}

void Renderer::clear(Color color) {

  HDC hdc = static_cast<HDC>(hdc_);
  if (!hdc) return;

  RECT rect{0, 0, width_, height_};

  HBRUSH brush = CreateSolidBrush(to_colorref(color));
  FillRect(hdc, &rect, brush);
  DeleteObject(brush);
}

void Renderer::draw_rect(int x, int y, int w, int h, Color fill, int radius) {

  HDC hdc = static_cast<HDC>(hdc_);
  if (!hdc) return;

  x += offset_x_;
  y += offset_y_;

  HBRUSH brush = CreateSolidBrush(to_colorref(fill));
  HPEN pen = CreatePen(PS_SOLID, 1, to_colorref(fill));

  HGDIOBJ old_brush = SelectObject(hdc, brush);
  HGDIOBJ old_pen = SelectObject(hdc, pen);

  RoundRect(hdc, x, y, x + w, y + h, radius, radius);

  SelectObject(hdc, old_brush);
  SelectObject(hdc, old_pen);

  DeleteObject(brush);
  DeleteObject(pen);
}

void Renderer::draw_rect_outline(int x, int y, int w, int h, Color stroke, int radius, int thickness) {
  HDC hdc = static_cast<HDC>(hdc_);
  if (!hdc) return;

  x += offset_x_;
  y += offset_y_;

  if (thickness <= 0) thickness = 1;

  HBRUSH brush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
  HPEN pen = CreatePen(PS_SOLID, thickness, to_colorref(stroke));

  HGDIOBJ old_brush = SelectObject(hdc, brush);
  HGDIOBJ old_pen = SelectObject(hdc, pen);

  RoundRect(hdc, x, y, x + w, y + h, radius, radius);

  SelectObject(hdc, old_brush);
  SelectObject(hdc, old_pen);

  DeleteObject(pen);
}

void Renderer::draw_line(int x1, int y1, int x2, int y2, Color stroke, int thickness) {
  HDC hdc = static_cast<HDC>(hdc_);
  if (!hdc) return;

  x1 += offset_x_;
  y1 += offset_y_;
  x2 += offset_x_;
  y2 += offset_y_;

  if (thickness <= 0) thickness = 1;
  HPEN pen = CreatePen(PS_SOLID, thickness, to_colorref(stroke));
  HGDIOBJ old_pen = SelectObject(hdc, pen);

  MoveToEx(hdc, x1, y1, nullptr);
  LineTo(hdc, x2, y2);

  SelectObject(hdc, old_pen);
  DeleteObject(pen);
}

void Renderer::draw_circle(int cx, int cy, int radius, Color fill, bool filled, int thickness) {
  HDC hdc = static_cast<HDC>(hdc_);
  if (!hdc) return;

  cx += offset_x_;
  cy += offset_y_;

  if (radius <= 0) return;
  if (thickness <= 0) thickness = 1;

  HBRUSH brush = filled ? CreateSolidBrush(to_colorref(fill)) : static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
  HPEN pen = CreatePen(PS_SOLID, filled ? 1 : thickness, to_colorref(fill));

  HGDIOBJ old_brush = SelectObject(hdc, brush);
  HGDIOBJ old_pen = SelectObject(hdc, pen);

  Ellipse(hdc, cx - radius, cy - radius, cx + radius, cy + radius);

  SelectObject(hdc, old_brush);
  SelectObject(hdc, old_pen);

  if (filled) {
    DeleteObject(brush);
  }
  DeleteObject(pen);
}

void Renderer::draw_text(int x, int y, const std::string& text, Color color, const std::string& font_family, int font_size) {

  HDC hdc = static_cast<HDC>(hdc_);
  if (!hdc) return;

  x += offset_x_;
  y += offset_y_;

  SetBkMode(hdc, TRANSPARENT);
  SetTextColor(hdc, to_colorref(color));

  HFONT font = get_font_cached(hdc, font_family, font_size);
  HGDIOBJ old_font = nullptr;
  if (font) {
    old_font = SelectObject(hdc, font);
  }

  TextOutA(hdc, x, y, text.c_str(), static_cast<int>(text.size()));

  if (old_font) {
    SelectObject(hdc, old_font);
  }
}

void Renderer::draw_image(int x, int y, int w, int h, const std::shared_ptr<Image>& image) {
  HDC hdc = static_cast<HDC>(hdc_);
  if (!hdc) return;
  if (!image || !image->native()) return;
  if (!gdiplus_token_) return;

  x += offset_x_;
  y += offset_y_;

  Gdiplus::Graphics g(hdc);
  g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
  g.DrawImage(image->native(), x, y, w, h);
}

void Renderer::push_offset(int dx, int dy) {
  offset_stack_.push_back({offset_x_, offset_y_});
  offset_x_ += dx;
  offset_y_ += dy;
}

void Renderer::pop_offset() {
  if (offset_stack_.empty()) return;
  auto prev = offset_stack_.back();
  offset_stack_.pop_back();
  offset_x_ = prev.first;
  offset_y_ = prev.second;
}

void Renderer::push_clip(int x, int y, int w, int h) {
  HDC hdc = static_cast<HDC>(hdc_);
  if (!hdc) return;

  const int left = x + offset_x_;
  const int top = y + offset_y_;

  SaveDC(hdc);
  ++clip_depth_;
  IntersectClipRect(hdc, left, top, left + w, top + h);
}

void Renderer::pop_clip() {
  HDC hdc = static_cast<HDC>(hdc_);
  if (!hdc) return;
  if (clip_depth_ <= 0) return;

  RestoreDC(hdc, -1);
  --clip_depth_;
}

void Renderer::draw_widget_tree(const std::shared_ptr<widgets::Widget>& root) {

  if (root) {
    root->draw(*this);
  }
}

}

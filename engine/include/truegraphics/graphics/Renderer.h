#pragma once

#include <memory>
#include <string>
#include <vector>
#include <windows.h>

namespace truegraphics::window {
class Window;
}

namespace truegraphics::widgets {
class Widget;
}

namespace truegraphics::graphics {

class Image;

struct Color {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;

  Color() : r(0), g(0), b(0), a(255) {}

  Color(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_ = 255)
      : r(r_), g(g_), b(b_), a(a_) {}
};

class Renderer {
 public:
  Renderer();
  ~Renderer();

  bool initialize(window::Window* window);

  void begin_frame(void* present_dc = nullptr);
  void end_frame();

  void clear(Color color);

  void draw_rect(int x, int y, int w, int h, Color fill, int radius = 0);
  void draw_rect_outline(int x, int y, int w, int h, Color stroke, int radius = 0, int thickness = 1);
  void draw_line(int x1, int y1, int x2, int y2, Color stroke, int thickness = 1);
  void draw_circle(int cx, int cy, int radius, Color fill, bool filled = true, int thickness = 1);
  void draw_text(
      int x,
      int y,
      const std::string& text,
      Color color,
      const std::string& font_family = "Segoe UI",
      int font_size = 16);
  void draw_image(int x, int y, int w, int h, const std::shared_ptr<Image>& image);

  void draw_widget_tree(const std::shared_ptr<widgets::Widget>& root);

  void push_offset(int dx, int dy);
  void pop_offset();

  void push_clip(int x, int y, int w, int h);
  void pop_clip();

 private:
  HWND hwnd_ = nullptr;

  HDC hdc_ = nullptr;

  // Double buffering
  HDC mem_dc_ = nullptr;
  HBITMAP mem_bitmap_ = nullptr;
  HGDIOBJ mem_old_bitmap_ = nullptr;
  HDC present_dc_ = nullptr;
  uintptr_t gdiplus_token_ = 0;

  int width_ = 0;
  int height_ = 0;

  int offset_x_ = 0;
  int offset_y_ = 0;
  std::vector<std::pair<int, int>> offset_stack_;
  int clip_depth_ = 0;
};

}

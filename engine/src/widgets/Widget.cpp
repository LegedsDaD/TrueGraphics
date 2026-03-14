#include "truegraphics/widgets/Widget.h"

#include "truegraphics/graphics/Renderer.h"

namespace truegraphics::widgets {

Widget::Widget() : id_(core::Uuid::generate()) {}
Widget::~Widget() = default;

const std::string& Widget::id() const { return id_; }
void Widget::set_id(std::string id) { id_ = std::move(id); }

void Widget::set_text(std::string text) { text_ = std::move(text); }
const std::string& Widget::text() const { return text_; }

void Widget::set_bounds(int x, int y, int w, int h) {
  x_ = x;
  y_ = y;
  width_ = w;
  height_ = h;
}

void Widget::set_position(int x, int y) {
  x_ = x;
  y_ = y;
}

void Widget::set_size(int w, int h) {
  width_ = w;
  height_ = h;
}

int Widget::x() const { return x_; }
int Widget::y() const { return y_; }
int Widget::width() const { return width_; }
int Widget::height() const { return height_; }

style::Style& Widget::style() { return style_; }
const style::Style& Widget::style() const { return style_; }

void Widget::set_on_click(std::function<void()> cb) { on_click_ = std::move(cb); }

void Widget::click() {
  if (on_click_) {
    on_click_();
  }
}

bool Widget::hit_test(int px, int py) const {
  return px >= x_ && py >= y_ && px <= x_ + width_ && py <= y_ + height_;
}

std::shared_ptr<Widget> Widget::hit_test_deep(int px, int py) {
  if (!hit_test(px, py)) {
    return nullptr;
  }

  for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
    if (auto hit = (*it)->hit_test_deep(px, py)) {
      return hit;
    }
  }

  return shared_from_this();
}

void Widget::set_focusable(bool focusable) { focusable_ = focusable; }
bool Widget::focusable() const { return focusable_; }

void Widget::set_focused(bool focused) { focused_ = focused; }
bool Widget::focused() const { return focused_; }

void Widget::set_on_hover(std::function<void(bool)> cb) { on_hover_ = std::move(cb); }
void Widget::set_on_mouse_move(std::function<void(int32_t, int32_t)> cb) { on_mouse_move_ = std::move(cb); }
void Widget::set_on_mouse_down(std::function<void(int32_t, int32_t)> cb) { on_mouse_down_ = std::move(cb); }
void Widget::set_on_mouse_up(std::function<void(int32_t, int32_t)> cb) { on_mouse_up_ = std::move(cb); }
void Widget::set_on_key_down(std::function<void(int32_t)> cb) { on_key_down_ = std::move(cb); }
void Widget::set_on_char(std::function<void(uint32_t)> cb) { on_char_ = std::move(cb); }
void Widget::set_on_drop_files(std::function<void(std::vector<std::string>)> cb) { on_drop_files_ = std::move(cb); }

void Widget::set_hovered(bool hovered) {
  if (hovered_ == hovered) {
    return;
  }
  hovered_ = hovered;
  if (on_hover_) {
    on_hover_(hovered_);
  }
}

bool Widget::hovered() const { return hovered_; }

void Widget::mouse_move(int32_t x, int32_t y) {
  if (on_mouse_move_) {
    on_mouse_move_(x, y);
  }
}

void Widget::mouse_down(int32_t x, int32_t y) {
  if (on_mouse_down_) {
    on_mouse_down_(x, y);
  }
}

void Widget::mouse_up(int32_t x, int32_t y) {
  if (on_mouse_up_) {
    on_mouse_up_(x, y);
  }
}

void Widget::key_down(int32_t key) {
  if (on_key_down_) {
    on_key_down_(key);
  }
}

void Widget::char_input(uint32_t codepoint) {
  if (on_char_) {
    on_char_(codepoint);
  }
}

void Widget::drop_files(std::vector<std::string> files) {
  if (on_drop_files_) {
    on_drop_files_(std::move(files));
  }
}

std::shared_ptr<Widget> Widget::parent() const { return parent_.lock(); }

void Widget::draw(graphics::Renderer& renderer) {
  renderer.draw_rect(x_, y_, width_, height_, style_.background, style_.corner_radius);
  if (style_.border_width > 0) {
    renderer.draw_rect_outline(x_, y_, width_, height_, style_.border, style_.corner_radius, style_.border_width);
  }
  for (const auto& child : children_) {
    child->draw(renderer);
  }
}

void Widget::add_child(const std::shared_ptr<Widget>& child) {
  children_.push_back(child);
  if (child) {
    child->parent_ = weak_from_this();
  }
}

const std::vector<std::shared_ptr<Widget>>& Widget::children() const { return children_; }

}  // namespace truegraphics::widgets

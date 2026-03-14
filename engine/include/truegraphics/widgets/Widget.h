#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "truegraphics/core/Uuid.h"
#include "truegraphics/graphics/Renderer.h"
#include "truegraphics/style/Style.h"

namespace truegraphics::widgets {

class Widget : public std::enable_shared_from_this<Widget> {
 public:
  Widget();
  virtual ~Widget();

  const std::string& id() const;
  void set_id(std::string id);

  void set_text(std::string text);
  const std::string& text() const;

  void set_bounds(int x, int y, int w, int h);
  void set_position(int x, int y);
  void set_size(int w, int h);

  int x() const;
  int y() const;
  int width() const;
  int height() const;

  style::Style& style();
  const style::Style& style() const;

  void set_on_click(std::function<void()> cb);
  void click();
  bool hit_test(int px, int py) const;
  virtual std::shared_ptr<Widget> hit_test_deep(int px, int py);

  void set_focusable(bool focusable);
  bool focusable() const;
  void set_focused(bool focused);
  bool focused() const;

  void set_on_hover(std::function<void(bool)> cb);
  void set_on_mouse_move(std::function<void(int32_t, int32_t)> cb);
  void set_on_mouse_down(std::function<void(int32_t, int32_t)> cb);
  void set_on_mouse_up(std::function<void(int32_t, int32_t)> cb);
  void set_on_key_down(std::function<void(int32_t)> cb);
  void set_on_char(std::function<void(uint32_t)> cb);
  void set_on_drop_files(std::function<void(std::vector<std::string>)> cb);

  void set_hovered(bool hovered);
  bool hovered() const;

  void mouse_move(int32_t x, int32_t y);
  void mouse_down(int32_t x, int32_t y);
  void mouse_up(int32_t x, int32_t y);
  void key_down(int32_t key);
  void char_input(uint32_t codepoint);
  void drop_files(std::vector<std::string> files);

  std::shared_ptr<Widget> parent() const;

  virtual void layout() {}
  virtual void draw(graphics::Renderer& renderer);
  virtual void add_child(const std::shared_ptr<Widget>& child);
  const std::vector<std::shared_ptr<Widget>>& children() const;

 protected:
  std::string id_;
  std::string text_;
  int x_ = 0;
  int y_ = 0;
  int width_ = 120;
  int height_ = 36;
  style::Style style_;
  std::function<void()> on_click_;
  std::function<void(bool)> on_hover_;
  std::function<void(int32_t, int32_t)> on_mouse_move_;
  std::function<void(int32_t, int32_t)> on_mouse_down_;
  std::function<void(int32_t, int32_t)> on_mouse_up_;
  std::function<void(int32_t)> on_key_down_;
  std::function<void(uint32_t)> on_char_;
  std::function<void(std::vector<std::string>)> on_drop_files_;
  bool hovered_ = false;
  bool focusable_ = false;
  bool focused_ = false;
  std::weak_ptr<Widget> parent_;
  std::vector<std::shared_ptr<Widget>> children_;
};

}  // namespace truegraphics::widgets

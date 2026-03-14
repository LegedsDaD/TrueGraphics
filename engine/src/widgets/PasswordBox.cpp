#include "truegraphics/widgets/PasswordBox.h"

#include <algorithm>
#include <cctype>
#include <cstring>

#include <windows.h>

namespace truegraphics::widgets {

PasswordBox::PasswordBox(const std::string& text) {
  set_text(text);
  set_size(320, 42);
  set_focusable(true);
  style_.use_gradient = false;

  caret_ = static_cast<int>(text_.size());
  sel_anchor_ = caret_;
  sel_start_ = caret_;
  sel_end_ = caret_;

  auto copy_text_to_clipboard = [](const std::string& s) {
    if (!OpenClipboard(nullptr)) return;
    EmptyClipboard();
    HGLOBAL hmem = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
    if (!hmem) {
      CloseClipboard();
      return;
    }
    void* p = GlobalLock(hmem);
    if (!p) {
      GlobalFree(hmem);
      CloseClipboard();
      return;
    }
    memcpy(p, s.c_str(), s.size() + 1);
    GlobalUnlock(hmem);
    if (!SetClipboardData(CF_TEXT, hmem)) {
      GlobalFree(hmem);
    }
    CloseClipboard();
  };

  auto read_clipboard_text = []() -> std::string {
    std::string out;
    if (!OpenClipboard(nullptr)) return out;
    HANDLE h = GetClipboardData(CF_TEXT);
    if (h) {
      char* ptr = static_cast<char*>(GlobalLock(h));
      if (ptr) {
        out = std::string(ptr);
        GlobalUnlock(h);
      }
    }
    CloseClipboard();
    return out;
  };

  set_on_char([this](uint32_t codepoint) {
    if (codepoint == 8) {
      key_down(VK_BACK);
      return;
    }
    if (codepoint >= 32 && codepoint < 127) {
      char ch = static_cast<char>(codepoint);
      if (std::isprint(static_cast<unsigned char>(ch))) {
        if (sel_start_ != sel_end_) {
          int a = sel_start_, b = sel_end_;
          if (a > b) std::swap(a, b);
          text_.erase(static_cast<size_t>(a), static_cast<size_t>(b - a));
          caret_ = a;
          sel_anchor_ = caret_;
          sel_start_ = caret_;
          sel_end_ = caret_;
        }
        caret_ = std::clamp(caret_, 0, static_cast<int>(text_.size()));
        text_.insert(text_.begin() + caret_, ch);
        caret_++;
        sel_anchor_ = caret_;
        sel_start_ = caret_;
        sel_end_ = caret_;
      }
    }
  });

  set_on_key_down([this, copy_text_to_clipboard, read_clipboard_text](int32_t key) {
    const bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    const bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

    auto clear_selection = [this]() {
      sel_anchor_ = caret_;
      sel_start_ = caret_;
      sel_end_ = caret_;
    };
    auto has_selection = [this]() { return sel_start_ != sel_end_; };
    auto normalize_sel = [this]() {
      int a = sel_start_, b = sel_end_;
      if (a > b) std::swap(a, b);
      return std::pair<int, int>(a, b);
    };
    auto delete_selection = [&, this]() {
      if (!has_selection()) return;
      auto [a, b] = normalize_sel();
      if (a < 0) a = 0;
      if (b > static_cast<int>(text_.size())) b = static_cast<int>(text_.size());
      if (b > a) {
        text_.erase(static_cast<size_t>(a), static_cast<size_t>(b - a));
        caret_ = a;
      }
      clear_selection();
    };

    auto begin_shift_selection = [this]() {
      if (sel_start_ == sel_end_) {
        sel_anchor_ = caret_;
      }
    };
    auto update_shift_selection = [this]() {
      sel_start_ = sel_anchor_;
      sel_end_ = caret_;
    };

    if (ctrl && key == 'A') {
      sel_anchor_ = 0;
      sel_start_ = 0;
      sel_end_ = static_cast<int>(text_.size());
      caret_ = sel_end_;
      return;
    }

    if (ctrl && key == 'C') {
      if (has_selection()) {
        auto [a, b] = normalize_sel();
        copy_text_to_clipboard(text_.substr(static_cast<size_t>(a), static_cast<size_t>(b - a)));
      } else {
        copy_text_to_clipboard(text_);
      }
      return;
    }

    if (ctrl && key == 'X') {
      if (has_selection()) {
        auto [a, b] = normalize_sel();
        copy_text_to_clipboard(text_.substr(static_cast<size_t>(a), static_cast<size_t>(b - a)));
        delete_selection();
      } else {
        copy_text_to_clipboard(text_);
        text_.clear();
        caret_ = 0;
        clear_selection();
      }
      return;
    }

    if (ctrl && key == 'V') {
      const std::string clip = read_clipboard_text();
      if (!clip.empty()) {
        if (has_selection()) {
          delete_selection();
        }
        caret_ = std::clamp(caret_, 0, static_cast<int>(text_.size()));
        text_.insert(static_cast<size_t>(caret_), clip);
        caret_ += static_cast<int>(clip.size());
        clear_selection();
      }
      return;
    }

    if (key == VK_LEFT) {
      if (shift) begin_shift_selection();
      caret_ = std::max(0, caret_ - 1);
      if (shift) update_shift_selection();
      else clear_selection();
      return;
    }
    if (key == VK_RIGHT) {
      if (shift) begin_shift_selection();
      caret_ = std::min(static_cast<int>(text_.size()), caret_ + 1);
      if (shift) update_shift_selection();
      else clear_selection();
      return;
    }
    if (key == VK_HOME) {
      if (shift) begin_shift_selection();
      caret_ = 0;
      if (shift) update_shift_selection();
      else clear_selection();
      return;
    }
    if (key == VK_END) {
      if (shift) begin_shift_selection();
      caret_ = static_cast<int>(text_.size());
      if (shift) update_shift_selection();
      else clear_selection();
      return;
    }

    if (key == VK_BACK) {
      if (has_selection()) {
        delete_selection();
        return;
      }
      if (caret_ > 0 && !text_.empty()) {
        text_.erase(text_.begin() + (caret_ - 1));
        caret_--;
        clear_selection();
      }
      return;
    }

    if (key == VK_DELETE) {
      if (has_selection()) {
        delete_selection();
        return;
      }
      if (caret_ >= 0 && caret_ < static_cast<int>(text_.size())) {
        text_.erase(text_.begin() + caret_);
        clear_selection();
      }
      return;
    }

    if (!shift) {
      clear_selection();
    }
  });
}

void PasswordBox::draw(graphics::Renderer& renderer) {
  renderer.draw_rect(x_, y_, width_, height_, style_.background, style_.corner_radius);
  auto border = style_.border;
  if (focused_) border = style_.foreground;
  if (style_.border_width > 0) {
    renderer.draw_rect_outline(x_, y_, width_, height_, border, style_.corner_radius, style_.border_width);
  }

  const int char_w = std::max(7, style_.font_size * 6 / 10);
  const int text_x = x_ + 12;
  const int text_y = y_ + 12;

  if (sel_start_ != sel_end_) {
    int a = sel_start_, b = sel_end_;
    if (a > b) std::swap(a, b);
    const int sx = text_x + a * char_w;
    const int sw = std::max(1, (b - a) * char_w);
    renderer.draw_rect(sx, text_y - 2, sw, style_.font_size + 8, {60, 110, 220, 120}, 6);
  }

  std::string masked(text_.size(), '*');
  renderer.draw_text(text_x, text_y, masked, style_.foreground, style_.font_family, style_.font_size);

  if (focused_) {
    const int caret_x = text_x + caret_ * char_w;
    renderer.draw_line(caret_x, y_ + 10, caret_x, y_ + height_ - 10, style_.foreground, 1);
  }
}

}  // namespace truegraphics::widgets

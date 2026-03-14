#include "truegraphics/widgets/Table.h"

#include <algorithm>

namespace truegraphics::widgets {

Table::Table(std::vector<std::string> headers, std::vector<std::vector<std::string>> rows)
    : headers_(std::move(headers)), rows_(std::move(rows)) {
  set_size(520, 240);
  style_.use_gradient = false;
  style_.border_width = 1;
  style_.corner_radius = 10;
}

void Table::set_headers(std::vector<std::string> headers) { headers_ = std::move(headers); }
void Table::set_rows(std::vector<std::vector<std::string>> rows) { rows_ = std::move(rows); }

void Table::draw(graphics::Renderer& renderer) {
  renderer.draw_rect(x_, y_, width_, height_, style_.background, style_.corner_radius);
  if (style_.border_width > 0) {
    renderer.draw_rect_outline(x_, y_, width_, height_, style_.border, style_.corner_radius, style_.border_width);
  }

  const int cols = std::max(1, static_cast<int>(headers_.size()));
  const int col_w = (width_ - style_.padding * 2) / cols;

  // header background
  renderer.draw_rect(x_ + 2, y_ + 2, width_ - 4, header_h_, {0, 0, 0, 50}, 8);

  for (int c = 0; c < cols; ++c) {
    const int cx = x_ + style_.padding + c * col_w;
    if (c < static_cast<int>(headers_.size())) {
      renderer.draw_text(cx + 6, y_ + 8, headers_[static_cast<size_t>(c)], style_.foreground, style_.font_family, style_.font_size);
    }
    // vertical separators
    if (c > 0) {
      renderer.draw_line(cx, y_ + 4, cx, y_ + height_ - 4, {0, 0, 0, 50}, 1);
    }
  }

  const int start_y = y_ + header_h_ + 6;
  for (int r = 0; r < static_cast<int>(rows_.size()); ++r) {
    const int ry = start_y + r * row_h_;
    if (ry > y_ + height_) break;
    for (int c = 0; c < cols; ++c) {
      const int cx = x_ + style_.padding + c * col_w;
      if (c < static_cast<int>(rows_[static_cast<size_t>(r)].size())) {
        renderer.draw_text(cx + 6, ry + 5, rows_[static_cast<size_t>(r)][static_cast<size_t>(c)], style_.foreground, style_.font_family, style_.font_size);
      }
    }
    renderer.draw_line(x_ + 6, ry + row_h_, x_ + width_ - 6, ry + row_h_, {0, 0, 0, 40}, 1);
  }
}

}  // namespace truegraphics::widgets

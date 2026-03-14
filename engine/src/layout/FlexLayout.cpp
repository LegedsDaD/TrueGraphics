#include "truegraphics/layout/FlexLayout.h"

#include <algorithm>
#include <vector>

#include "truegraphics/widgets/Container.h"
#include "truegraphics/widgets/Widget.h"

namespace truegraphics::layout {

void FlexLayout::apply(const std::shared_ptr<widgets::Container>& root) {
  apply_widget(root);
}

void FlexLayout::apply_widget(const std::shared_ptr<widgets::Widget>& widget) {
  if (!widget) return;

  if (auto container = std::dynamic_pointer_cast<widgets::Container>(widget)) {
    layout_container(container);
  }

  for (const auto& child : widget->children()) {
    apply_widget(child);
  }
}

void FlexLayout::layout_container(const std::shared_ptr<widgets::Container>& container) {
  if (!container) return;

  const int padding = container->style().padding;
  const int gap = container->gap();

  const int inner_x = container->x() + padding;
  const int inner_y = container->y() + padding;
  const int inner_w = std::max(0, container->width() - padding * 2);
  const int inner_h = std::max(0, container->height() - padding * 2);

  const auto& kids = container->children();
  const int count = static_cast<int>(kids.size());
  if (count == 0) return;

  auto layout_mode = container->layout_mode();
  auto align = container->align();
  auto justify = container->justify();

  if (layout_mode == widgets::Container::LayoutMode::None) return;

  if (layout_mode == widgets::Container::LayoutMode::Row) {
    int total_w = 0;
    for (const auto& c : kids) {
      const int m = c->style().margin;
      total_w += c->width() + m * 2;
    }
    total_w += gap * (count - 1);

    int cursor_x = inner_x;
    int effective_gap = gap;
    if (justify == widgets::Container::Justify::Center) {
      cursor_x = inner_x + (inner_w - total_w) / 2;
    } else if (justify == widgets::Container::Justify::End) {
      cursor_x = inner_x + (inner_w - total_w);
    } else if (justify == widgets::Container::Justify::SpaceBetween && count > 1) {
      effective_gap = (inner_w - (total_w - gap * (count - 1))) / (count - 1);
      cursor_x = inner_x;
    }

    for (const auto& c : kids) {
      const int m = c->style().margin;

      int child_h = c->height();
      int child_y = inner_y + m;
      if (align == widgets::Container::Align::Center) {
        child_y = inner_y + (inner_h - child_h) / 2;
      } else if (align == widgets::Container::Align::End) {
        child_y = inner_y + inner_h - child_h - m;
      } else if (align == widgets::Container::Align::Stretch) {
        child_h = std::max(0, inner_h - m * 2);
        c->set_size(c->width(), child_h);
        child_y = inner_y + m;
      }

      c->set_position(cursor_x + m, child_y);
      cursor_x += c->width() + m * 2 + effective_gap;

    }

    return;
  }

  if (layout_mode == widgets::Container::LayoutMode::Grid) {
    int cols = container->grid_columns();
    if (cols <= 0) cols = 1;

    const int cell_w = cols == 0 ? inner_w : (inner_w - gap * (cols - 1)) / cols;
    const int rows = (count + cols - 1) / cols;

    std::vector<int> row_heights(rows, 0);
    for (int i = 0; i < count; ++i) {
      const auto& c = kids[static_cast<size_t>(i)];
      const int m = c->style().margin;
      const int r = i / cols;
      row_heights[static_cast<size_t>(r)] = std::max(row_heights[static_cast<size_t>(r)], c->height() + m * 2);
    }

    int y = inner_y;
    for (int r = 0, i = 0; r < rows; ++r) {
      int x = inner_x;
      for (int col = 0; col < cols && i < count; ++col, ++i) {
        const auto& c = kids[static_cast<size_t>(i)];
        const int m = c->style().margin;

        int child_w = c->width();
        int child_h = c->height();
        if (align == widgets::Container::Align::Stretch) {
          child_w = std::max(0, cell_w - m * 2);
          c->set_size(child_w, child_h);
        }

        c->set_position(x + m, y + m);
        x += cell_w + gap;

      }
      y += row_heights[static_cast<size_t>(r)] + gap;
    }

    return;
  }

  // Column (default)
  int total_h = 0;
  for (const auto& c : kids) {
    const int m = c->style().margin;
    total_h += c->height() + m * 2;
  }
  total_h += gap * (count - 1);

  int cursor_y = inner_y;
  int effective_gap = gap;
  if (justify == widgets::Container::Justify::Center) {
    cursor_y = inner_y + (inner_h - total_h) / 2;
  } else if (justify == widgets::Container::Justify::End) {
    cursor_y = inner_y + (inner_h - total_h);
  } else if (justify == widgets::Container::Justify::SpaceBetween && count > 1) {
    effective_gap = (inner_h - (total_h - gap * (count - 1))) / (count - 1);
    cursor_y = inner_y;
  }

  for (const auto& c : kids) {
    const int m = c->style().margin;

    int child_w = c->width();
    int child_x = inner_x + m;
    if (align == widgets::Container::Align::Center) {
      child_x = inner_x + (inner_w - child_w) / 2;
    } else if (align == widgets::Container::Align::End) {
      child_x = inner_x + inner_w - child_w - m;
    } else if (align == widgets::Container::Align::Stretch) {
      child_w = std::max(0, inner_w - m * 2);
      c->set_size(child_w, c->height());
      child_x = inner_x + m;
    }

    c->set_position(child_x, cursor_y + m);
    cursor_y += c->height() + m * 2 + effective_gap;

  }
}

}  // namespace truegraphics::layout

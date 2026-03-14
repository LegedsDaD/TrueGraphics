#pragma once

#include <functional>
#include <string>
#include <vector>

#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class Table final : public Widget {
 public:
  Table(std::vector<std::string> headers = {}, std::vector<std::vector<std::string>> rows = {});

  void set_headers(std::vector<std::string> headers);
  void set_rows(std::vector<std::vector<std::string>> rows);

  void draw(graphics::Renderer& renderer) override;

 private:
  std::vector<std::string> headers_;
  std::vector<std::vector<std::string>> rows_;
  int row_h_ = 24;
  int header_h_ = 28;
};

}  // namespace truegraphics::widgets


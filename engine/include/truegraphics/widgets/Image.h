#pragma once

#include <string>

#include "truegraphics/graphics/Image.h"
#include "truegraphics/widgets/Widget.h"

namespace truegraphics::widgets {

class Image final : public Widget {
 public:
  explicit Image(const std::string& path = "");
  void set_source(const std::string& path);
  const std::string& source() const;

  void draw(graphics::Renderer& renderer) override;

 private:
  std::string source_;
  std::shared_ptr<graphics::Image> image_;
};

}  // namespace truegraphics::widgets


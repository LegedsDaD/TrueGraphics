#include "truegraphics/widgets/Image.h"

#include <string>

#include "truegraphics/resource/Resources.h"

namespace truegraphics::widgets {

namespace {
std::wstring to_wide(const std::string& input) { return std::wstring(input.begin(), input.end()); }
}

Image::Image(const std::string& path) {
  set_size(240, 160);
  style_.use_gradient = false;
  set_source(path);
}

void Image::set_source(const std::string& path) {
  source_ = resource::resolve(path);
  image_ = nullptr;

  if (!source_.empty()) {
    image_ = graphics::load_image_cached(to_wide(source_));
    if (image_ && (width_ <= 0 || height_ <= 0)) {
      set_size(image_->width(), image_->height());
    }
  }
}

const std::string& Image::source() const { return source_; }

void Image::draw(graphics::Renderer& renderer) {
  renderer.draw_rect(x_, y_, width_, height_, style_.background, style_.corner_radius);
  if (style_.border_width > 0) {
    renderer.draw_rect_outline(x_, y_, width_, height_, style_.border, style_.corner_radius, style_.border_width);
  }
  renderer.draw_image(x_, y_, width_, height_, image_);
}

}  // namespace truegraphics::widgets

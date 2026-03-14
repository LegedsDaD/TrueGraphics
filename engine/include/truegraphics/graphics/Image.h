#pragma once

#include <memory>
#include <string>

namespace Gdiplus {
class Image;
}

namespace truegraphics::graphics {

class Image final {
 public:
  Image();
  ~Image();

  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;

  bool load(const std::wstring& path);
  int width() const;
  int height() const;
  const std::wstring& path() const;

  Gdiplus::Image* native() const;

 private:
  std::unique_ptr<Gdiplus::Image> image_;
  int width_ = 0;
  int height_ = 0;
  std::wstring path_;
};

std::shared_ptr<Image> load_image_cached(const std::wstring& path);

}  // namespace truegraphics::graphics


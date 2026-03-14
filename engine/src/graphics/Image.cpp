#include "truegraphics/graphics/Image.h"

#include <objidl.h>
#include <gdiplus.h>

#include <mutex>
#include <unordered_map>

namespace truegraphics::graphics {

Image::Image() = default;
Image::~Image() = default;

bool Image::load(const std::wstring& path) {
  path_ = path;
  image_.reset();
  width_ = 0;
  height_ = 0;

  if (path.empty()) return false;

  auto img = std::make_unique<Gdiplus::Image>(path.c_str(), FALSE);
  if (!img || img->GetLastStatus() != Gdiplus::Ok) {
    return false;
  }

  width_ = static_cast<int>(img->GetWidth());
  height_ = static_cast<int>(img->GetHeight());
  image_ = std::move(img);
  return true;
}

int Image::width() const { return width_; }
int Image::height() const { return height_; }
const std::wstring& Image::path() const { return path_; }
Gdiplus::Image* Image::native() const { return image_.get(); }

std::shared_ptr<Image> load_image_cached(const std::wstring& path) {
  static std::mutex g_mutex;
  static std::unordered_map<std::wstring, std::weak_ptr<Image>> g_cache;

  if (path.empty()) return nullptr;

  std::lock_guard<std::mutex> lock(g_mutex);

  if (auto it = g_cache.find(path); it != g_cache.end()) {
    if (auto existing = it->second.lock()) {
      return existing;
    }
  }

  auto img = std::make_shared<Image>();
  if (!img->load(path)) {
    return nullptr;
  }

  g_cache[path] = img;
  return img;
}

}  // namespace truegraphics::graphics

#include "truegraphics/resource/Resources.h"

#include <filesystem>
#include <mutex>

#include <windows.h>

namespace truegraphics::resource {

namespace fs = std::filesystem;

namespace {
std::mutex g_mutex;
std::vector<std::string> g_paths;

std::wstring to_wide(const std::string& input) { return std::wstring(input.begin(), input.end()); }

bool exists_file(const fs::path& p) {
  std::error_code ec;
  return fs::exists(p, ec) && fs::is_regular_file(p, ec);
}
}

void add_path(const std::string& path) {
  std::lock_guard<std::mutex> lock(g_mutex);
  g_paths.push_back(path);
}

void clear_paths() {
  std::lock_guard<std::mutex> lock(g_mutex);
  g_paths.clear();
}

std::vector<std::string> paths() {
  std::lock_guard<std::mutex> lock(g_mutex);
  return g_paths;
}

std::string resolve(const std::string& path) {
  if (path.empty()) return path;

  fs::path p(path);
  if (exists_file(p)) {
    return path;
  }

  std::lock_guard<std::mutex> lock(g_mutex);
  for (const auto& base : g_paths) {
    fs::path candidate = fs::path(base) / p;
    if (exists_file(candidate)) {
      return candidate.string();
    }
  }

  return path;
}

bool load_font(const std::string& path) {
  const std::string resolved = resolve(path);
  std::wstring wide = to_wide(resolved);
  const int added = AddFontResourceExW(wide.c_str(), FR_PRIVATE, nullptr);
  if (added > 0) {
    SendMessageW(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
  }
  return added > 0;
}

bool unload_font(const std::string& path) {
  const std::string resolved = resolve(path);
  std::wstring wide = to_wide(resolved);
  const BOOL ok = RemoveFontResourceExW(wide.c_str(), FR_PRIVATE, nullptr);
  if (ok) {
    SendMessageW(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
  }
  return ok != FALSE;
}

}  // namespace truegraphics::resource


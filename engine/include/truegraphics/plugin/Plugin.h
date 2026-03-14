#pragma once

#include <string>
#include <vector>

namespace truegraphics::plugin {

class PluginManager final {
 public:
  bool load(const std::string& path);
  const std::vector<std::string>& loaded() const;

 private:
  std::vector<std::string> loaded_;
};

}  // namespace truegraphics::plugin

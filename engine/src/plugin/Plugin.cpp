#include "truegraphics/plugin/Plugin.h"

namespace truegraphics::plugin {

bool PluginManager::load(const std::string& path) {
  loaded_.push_back(path);
  return true;
}

const std::vector<std::string>& PluginManager::loaded() const { return loaded_; }

}  // namespace truegraphics::plugin

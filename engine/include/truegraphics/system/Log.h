#pragma once

#include <string>

namespace truegraphics::system {

class Log final {
 public:
  static void info(const std::string& message);
  static void warn(const std::string& message);
  static void error(const std::string& message);
};

}  // namespace truegraphics::system

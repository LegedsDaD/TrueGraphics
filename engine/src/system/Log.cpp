#include "truegraphics/system/Log.h"

#include <iostream>

namespace truegraphics::system {

static void write(const char* level, const std::string& message) {
  std::cout << "[TrueGraphics][" << level << "] " << message << std::endl;
}

void Log::info(const std::string& message) { write("INFO", message); }
void Log::warn(const std::string& message) { write("WARN", message); }
void Log::error(const std::string& message) { write("ERROR", message); }

}  // namespace truegraphics::system

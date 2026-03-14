#pragma once

#include <cstdint>
#include <string>

namespace truegraphics::core {

class Uuid final {
 public:
  static std::string generate();

 private:
  static std::uint64_t next_value();
};

}  // namespace truegraphics::core

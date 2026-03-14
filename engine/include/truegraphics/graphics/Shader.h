#pragma once

#include <string>

namespace truegraphics::graphics {

class Shader final {
 public:
  Shader() = default;
  explicit Shader(std::string source) : source_(std::move(source)) {}
  const std::string& source() const { return source_; }

 private:
  std::string source_;
};

}  // namespace truegraphics::graphics

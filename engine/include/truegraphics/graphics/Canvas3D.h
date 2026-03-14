#pragma once

namespace truegraphics::graphics {

class Canvas3D final {
 public:
  void set_rotation(float x, float y, float z);
  float rotation_x() const;
  float rotation_y() const;
  float rotation_z() const;

 private:
  float rx_ = 0.0f;
  float ry_ = 0.0f;
  float rz_ = 0.0f;
};

}  // namespace truegraphics::graphics

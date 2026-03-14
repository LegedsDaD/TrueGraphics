#include "truegraphics/graphics/Canvas3D.h"

namespace truegraphics::graphics {

void Canvas3D::set_rotation(float x, float y, float z) {
  rx_ = x;
  ry_ = y;
  rz_ = z;
}

float Canvas3D::rotation_x() const { return rx_; }
float Canvas3D::rotation_y() const { return ry_; }
float Canvas3D::rotation_z() const { return rz_; }

}  // namespace truegraphics::graphics

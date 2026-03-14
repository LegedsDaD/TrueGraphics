#include "truegraphics/animation/Easing.h"

namespace truegraphics::animation {

double Easing::smoothstep(double t) { return t * t * (3.0 - 2.0 * t); }

}  // namespace truegraphics::animation

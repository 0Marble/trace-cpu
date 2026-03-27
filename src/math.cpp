
#include "math.h"
#include <cmath>

namespace Math {

glm::vec3 fromSpherical(glm::vec2 uv) {
  float cos_u = std::cos(uv.x);
  float sin_u = std::sin(uv.x);
  float cos_v = std::cos(uv.y);
  float sin_v = std::sin(uv.y);

  return {
      sin_u * cos_v,
      sin_u * sin_v,
      cos_u,
  };
}

} // namespace Math

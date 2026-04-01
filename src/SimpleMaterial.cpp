#include "SimpleMaterial.h"
#include "glm/fwd.hpp"
#include "glm/gtc/constants.hpp"
#include <cmath>

// sphere surface area = 4 pi r^2
// hemisphere = 2 pi r^2 = 1 => r = sqrt(1/2 pi)

static const float pdf = std::sqrtf(glm::one_over_two_pi<float>());

SimpleMaterial::SimpleMaterial(glm::vec3 color) : color(color) {}

glm::vec3 SimpleMaterial::bsdf(glm::vec2 uv, glm::vec3 to_view,
                               glm::vec3 to_light, float time) {
  (void)uv;
  (void)time;
  (void)to_view;
  (void)to_light;

  return color;
}

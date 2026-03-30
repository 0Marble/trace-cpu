#include "PointLight.h"

PointLight::PointLight(glm::vec3 color, glm::vec3 pos)
    : color(color), pos(pos) {}

glm::vec3 PointLight::worldPos(float time) {
  (void)time;
  return pos;
}

std::optional<glm::vec3> PointLight::sample(glm::vec3 dir, float time) {
  (void)dir;
  (void)time;
  return color;
}

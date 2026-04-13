#pragma once

#include "Light.h"

class PointLight : public Light {
public:
  glm::vec3 color = {1, 1, 1};
  glm::vec3 pos = {0, 0, 0};

  PointLight() = default;
  PointLight(glm::vec3 color, glm::vec3 pos);

  glm::vec3 worldPos(float time) override;
  std::optional<glm::vec3> sample(glm::vec3 dir, float time) override;
};

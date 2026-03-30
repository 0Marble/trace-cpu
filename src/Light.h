#pragma once

#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <optional>

class Light {
public:
  virtual glm::vec3 worldPos(float time) = 0;
  virtual std::optional<glm::vec3> sample(glm::vec3 dir, float time) = 0;
};

#pragma once

#include "Ray.h"
#include <glm/glm.hpp>

class AABB {
public:
  glm::vec3 pos = {}, size = {};

  AABB combine(AABB other) const;
  bool contains(glm::vec3 point) const;
  std::array<glm::vec3, 8> corners() const;

  bool intersects(Ray ray) const;
};

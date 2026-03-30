#pragma once

#include <glm/glm.hpp>

class Ray {
public:
  glm::vec3 origin = {};
  glm::vec3 dir = {0, 0, -1};
  float time = 0;

  glm::vec3 at(float t) const;
};

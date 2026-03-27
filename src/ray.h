#pragma once

#include <glm/glm.hpp>

class Ray {
public:
  glm::vec3 origin;
  glm::vec3 dir;
  float time;

  glm::vec3 at(float t) const;
};

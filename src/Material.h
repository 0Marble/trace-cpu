
#pragma once

#include <glm/glm.hpp>

class Material {
public:
  virtual glm::vec2 sampleReflectedDir(glm::vec2 incoming, glm::vec2 uv,
                                       float time) = 0;
  virtual glm::vec3 sampleColor(glm::vec2 uv, float time) = 0;
};

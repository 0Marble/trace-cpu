
#pragma once

#include "Random.h"
#include <glm/glm.hpp>

class Material {
public:
  virtual glm::vec3 sampleReflectedDir(Random &rng, glm::vec3 incoming,
                                       glm::vec2 uv, float time) = 0;
  virtual glm::vec3 sampleColor(Random &rng, glm::vec3 to_light,
                                glm::vec3 to_view, glm::vec3 light_color,
                                glm::vec2 uv, float time) = 0;
};

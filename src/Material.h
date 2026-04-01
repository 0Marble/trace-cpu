
#pragma once

#include <glm/glm.hpp>

class Material {
public:
  virtual glm::vec3 bsdf(glm::vec2 uv, glm::vec3 to_view, glm::vec3 to_light,
                         float time) = 0;
};

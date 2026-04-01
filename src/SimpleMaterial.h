#pragma once

#include "Material.h"
#include "glm/fwd.hpp"

class SimpleMaterial : public Material {
public:
  glm::vec3 color;

  SimpleMaterial(glm::vec3 color);

  glm::vec3 bsdf(glm::vec2 uv, glm::vec3 to_view, glm::vec3 to_light,
                 float time) override;
};

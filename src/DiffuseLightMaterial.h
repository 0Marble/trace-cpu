#pragma once

#include "Material.h"
#include <glm/glm.hpp>

class DiffuseLightMaterial : public Material {

public:
  glm::vec3 color = {1.0f, 1.0f, 1.0f};

  glm::vec3 sampleReflectedDir(Random &rng, glm::vec3 incoming, glm::vec2 uv,
                               float time) override;
  glm::vec3 sampleColor(Random &rng, glm::vec3 to_light, glm::vec3 to_view,
                        glm::vec3 light_color, glm::vec2 uv,
                        float time) override;
};

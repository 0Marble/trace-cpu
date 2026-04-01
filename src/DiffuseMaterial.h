#pragma once

#include "Material.h"
#include "glm/fwd.hpp"

class DiffuseMaterial : public Material {
public:
  glm::vec3 color;

  DiffuseMaterial(glm::vec3 color);

  glm::vec3 bsdf(glm::vec2 uv, float time, glm::vec3 to_view,
                 glm::vec3 to_light) override;

  float pdf(glm::vec2 uv, float time, glm::vec3 to_view,
            glm::vec3 to_light) override;

  glm::vec3 sample(glm::vec2 uv, float time, glm::vec3 to_view, float &pdf_out,
                   glm::vec3 &bsdf_out) override;
};

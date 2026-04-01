
#pragma once

#include <glm/glm.hpp>

class Material {
public:
  virtual glm::vec3 bsdf(glm::vec2 uv, float time, glm::vec3 to_view,
                         glm::vec3 to_light) = 0;

  virtual float pdf(glm::vec2 uv, float time, glm::vec3 to_view,
                    glm::vec3 to_light) = 0;

  virtual glm::vec3 sample(glm::vec2 uv, float time, glm::vec3 to_view,
                           float &pdf_out, glm::vec3 &bsdf_out) = 0;
};

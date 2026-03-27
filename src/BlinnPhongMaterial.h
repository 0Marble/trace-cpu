#pragma once

#include "Material.h"
#include "Random.h"
#include <memory>

class BlinnPhongMaterial : public Material {
  glm::vec3 diffuse, specular;
  float shininess;
  std::shared_ptr<Random> rng;

public:
  glm::vec3 sampleReflectedDir(glm::vec3 incoming, glm::vec2 uv,
                               float time) override;

  glm::vec3 sampleColor(glm::vec3 incoming_dir, glm::vec3 outgoing_dir,
                        glm::vec3 incoming_color, glm::vec2 uv,
                        float time) override;
};

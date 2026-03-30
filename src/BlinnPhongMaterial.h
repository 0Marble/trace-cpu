#pragma once

#include "Material.h"
#include "Random.h"

class BlinnPhongMaterial : public Material {
public:
  glm::vec3 diffuse, specular;
  float shininess;

  BlinnPhongMaterial(glm::vec3 diffuse, glm::vec3 specular, float shininess);

  glm::vec3 sampleReflectedDir(Random &rng, glm::vec3 incoming, glm::vec2 uv,
                               float time) override;
  glm::vec3 sampleColor(Random &rng, glm::vec3 to_light, glm::vec3 to_view,
                        glm::vec3 light_color, glm::vec2 uv,
                        float time) override;
};


#pragma once

#include <glm/glm.hpp>

class Material {
public:
  virtual glm::vec3 sampleReflectedDir(glm::vec3 incoming, glm::vec2 uv,
                                       float time) = 0;
  virtual glm::vec3 sampleColor(glm::vec3 incoming_dir, glm::vec3 outgoing_dir,
                                glm::vec3 incoming_color, glm::vec2 uv,
                                float time) = 0;
};

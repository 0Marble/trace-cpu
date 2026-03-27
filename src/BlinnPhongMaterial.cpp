#include "BlinnPhongMaterial.h"
#include "glm/common.hpp"
#include "glm/exponential.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "math.h"

glm::vec3 BlinnPhongMaterial::sampleReflectedDir(glm::vec3 incoming,
                                                 glm::vec2 uv, float time) {
  (void)incoming;
  (void)uv;
  (void)time;
  return rng->hemisphereUniform();
}

glm::vec3 BlinnPhongMaterial::sampleColor(glm::vec3 incoming_dir,
                                          glm::vec3 outgoing_dir,
                                          glm::vec3 incoming_color,
                                          glm::vec2 uv, float time) {
  (void)uv;
  (void)time;

  glm::vec3 to_light = Math::fromSpherical(incoming_dir);
  glm::vec3 to_view = Math::fromSpherical(outgoing_dir);
  glm::vec3 normal = {0, 0, 1};

  float l = glm::max(glm::dot(to_light, normal), 0.0f);
  float s = 0.0f;

  if (l > 0.0f) {
    glm::vec3 half_dir = glm::normalize(to_light + to_view);
    float spec_angle = glm::max(glm::dot(half_dir, normal), 0.0f);
    s = glm::pow(spec_angle, shininess);
  }

  return (diffuse * l + specular * s) * incoming_color;
}

#include "BlinnPhongMaterial.h"
#include "Random.h"
#include "glm/common.hpp"
#include "glm/exponential.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"

BlinnPhongMaterial::BlinnPhongMaterial(glm::vec3 diffuse, glm::vec3 specular,
                                       float shininess)
    : diffuse(diffuse), specular(specular), shininess(shininess) {}

glm::vec3 BlinnPhongMaterial::sampleReflectedDir(Random &rng,
                                                 glm::vec3 incoming,
                                                 glm::vec2 uv, float time) {
#if 1
  (void)incoming;
  (void)uv;
  (void)time;
  return rng.hemisphereUniform();
#else
  (void)time;
  (void)uv;
  (void)rng;

  glm::vec3 normal = {0, 0, 1};
  return glm::reflect(incoming, normal);
#endif
}

glm::vec3 BlinnPhongMaterial::sampleColor(Random &rng, glm::vec3 to_light,
                                          glm::vec3 to_view,
                                          glm::vec3 light_color, glm::vec2 uv,
                                          float time) {
  (void)uv;
  (void)time;
  (void)rng;

  glm::vec3 normal = {0, 0, 1};

  to_light = glm::normalize(to_light);
  to_view = glm::normalize(to_view);
  float l = glm::max(glm::dot(to_light, normal), 0.0f);
  float s = 0.0f;

  if (l > 0.0f) {
    glm::vec3 half_dir = glm::normalize(to_light + to_view);
    float spec_angle = glm::max(glm::dot(half_dir, normal), 0.0f);
    s = glm::pow(spec_angle, shininess);
  }

  return (diffuse * l + specular * s) * light_color;
}

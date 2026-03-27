
#include "DiffuseLightMaterial.h"

glm::vec3 DiffuseLightMaterial::sampleReflectedDir(Random &rng,
                                                   glm::vec3 incoming,
                                                   glm::vec2 uv, float time) {
  (void)incoming;
  (void)uv;
  (void)time;
  return rng.hemisphereUniform();
}

glm::vec3 DiffuseLightMaterial::sampleColor(Random &rng, glm::vec3 to_light,
                                            glm::vec3 to_view,
                                            glm::vec3 light_color, glm::vec2 uv,
                                            float time) {
  (void)uv;
  (void)time;
  (void)rng;
  (void)to_view;
  (void)to_light;
  (void)light_color;

  return color;
}

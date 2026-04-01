#include "DiffuseMaterial.h"
#include "glm/fwd.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/random.hpp"
#include <cmath>

// sphere surface area = 4 pi r^2
// hemisphere = 2 pi r^2 = 1 => r = sqrt(1/2 pi)

static const float PDF = std::sqrtf(glm::one_over_two_pi<float>());

DiffuseMaterial::DiffuseMaterial(glm::vec3 color) : color(color) {}

glm::vec3 DiffuseMaterial::bsdf(glm::vec2 uv, float time, glm::vec3 to_view,
                                glm::vec3 to_light) {
  (void)uv;
  (void)time;
  (void)to_view;
  (void)to_light;

  return color;
}

float DiffuseMaterial::pdf(glm::vec2 uv, float time, glm::vec3 to_view,
                           glm::vec3 to_light) {
  (void)uv;
  (void)time;
  (void)to_light;
  (void)to_view;
  return PDF;
}

glm::vec3 DiffuseMaterial::sample(glm::vec2 uv, float time, glm::vec3 to_view,
                                  float &pdf_out, glm::vec3 &bsdf_out) {
  (void)uv;
  (void)time;
  (void)to_view;

  glm::vec3 res = glm::sphericalRand(1.0f);
  res.z = std::fabs(res.z);

  pdf_out = PDF;
  bsdf_out = color;

  return res;
};

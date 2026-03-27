#include "Sphere.h"
#include "glm/ext/scalar_common.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/constants.hpp"
#include <cmath>

std::optional<float> intersectT(Ray ray) {
  // x^2+y^2+z^2 = 1
  // o + td = (x, y, z)
  // <o + td, o + td> = 1
  // <o, o> + 2t<o, d> + t^2<d, d> = 1

  float a = glm::dot(ray.origin, ray.origin) - 1.0f;
  float b = 2.0f * glm::dot(ray.origin, ray.dir);
  float c = glm::dot(ray.dir, ray.dir);

  if (a == 0.0) {
    // NOTE:
    // we are probably not interested in t=0, since it would cause
    // self-intersection.

    if (b == 0.0)
      return {};

    return c / b;
  }

  float d = b * b - 4.0f * a * c;
  if (d < 0.0) {
    return {};
  }
  float d_sqrt = std::sqrt(d);

  float t1 = (-b - d_sqrt) / (2.0f * a);
  float t2 = (-b + d_sqrt) / (2.0f * a);
  float t = glm::fmax(t1, t2, 0.0f);

  if (t < 0.0)
    return {};

  return t;
}

std::optional<Collision> Sphere::intersect(Ray ray) {

  auto maybe_t = intersectT(ray);
  if (!maybe_t)
    return {};
  float t = maybe_t.value();

  glm::vec3 pos = ray.at(t);
  float u = glm::one_over_two_pi<float>() * std::atan2(pos.y, pos.x);
  float v = 1.0f - glm::one_over_pi<float>() * std::acos(pos.z);
  glm::vec3 tangent = {-pos.y, pos.x, 0};

  if (pos.z == 1.0f) {
    tangent = {1, 0, 0};
  } else if (pos.z == -1.0f) {
    tangent = {-1, 0, 0};
  } else {
    tangent = glm::normalize(tangent);
  }

  return Collision{
      .ray = ray,
      .t = t,
      .uv = {u, v},
      .normal = pos,
      .tangent = tangent,
      .pos = pos,
  };
}

AABB Sphere::aabb() {
  return {.pos = glm::vec3(-1.0f), .size = glm::vec3(2.0f)};
}

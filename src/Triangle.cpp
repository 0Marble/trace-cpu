#include "Triangle.h"
#include "Geometry.h"
#include "Log.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include <optional>

static const float epsilon = 1e-5;

Triangle::Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) : points({a, b, c}) {}

std::optional<glm::vec4> Triangle::intersectUVWT(Ray ray) {
  glm::vec3 e1 = points[1] - points[0];
  glm::vec3 e2 = points[2] - points[0];

  glm::vec3 dir_x_e2 = glm::cross(ray.dir, e2);
  float det = glm::dot(e1, dir_x_e2);
  if (glm::abs(det) < epsilon)
    return {};

  glm::vec3 s = ray.origin - points[0];
  float u = glm::dot(s, dir_x_e2) / det;
  if (u < -epsilon || u - 1.0f > epsilon)
    return {};

  glm::vec3 s_x_e1 = glm::cross(s, e1);
  float v = glm::dot(ray.dir, s_x_e1) / det;

  if (v < -epsilon || u + v - 1.0f > epsilon)
    return {};

  float t = glm::dot(e2, s_x_e1) / det;
  if (t > epsilon) {
    return glm::vec4(u, v, 1.0f - u - v, t);
  } else {
    return {};
  }
}

std::optional<Collision> Triangle::intersect(Ray ray) {
  auto maybe = intersectUVWT(ray);
  if (!maybe)
    return {};
  glm::vec4 uvwt = maybe.value();

  glm::vec3 e1 = points[1] - points[0];
  glm::vec3 e2 = points[2] - points[0];
  glm::vec3 n = glm::normalize(glm::cross(e1, e2));

  return (Collision){
      .ray = ray,
      .t = uvwt.w,
      .uv = glm::vec2(uvwt.x, uvwt.y),
      .normal = n,
      .tangent = glm::normalize(e1),
      .pos = uvwt.z * points[0] + uvwt.x * points[1] + uvwt.y * points[2],
  };
}

AABB Triangle::aabb() { TODO("unimplemented"); }

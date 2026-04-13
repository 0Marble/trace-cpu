#include "Triangle.h"
#include "Intersection.h"
#include "Transform.h"
#include "glm/common.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <array>
#include <memory>
#include <optional>

static const float epsilon = 1e-5;

Triangle::Triangle(const Object *obj, size_t face) : obj(obj), face(face) {}

std::array<glm::vec3, 3> Triangle::points() const {
  auto idx = obj->mesh->mesh->indices[face].vertex_index;
  auto a = obj->mesh->mesh->indices[3 * idx + 0].vertex_index;
  auto b = obj->mesh->mesh->indices[3 * idx + 1].vertex_index;
  auto c = obj->mesh->mesh->indices[3 * idx + 2].vertex_index;
  const auto &verts = obj->mesh->file->GetAttrib().vertices;

  return {
      glm::make_vec3(&verts[3 * a]),
      glm::make_vec3(&verts[3 * b]),
      glm::make_vec3(&verts[3 * c]),
  };
}

std::optional<glm::vec4> Triangle::intersectUVWT(Ray ray) const {
  auto points = this->points();
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

std::optional<Intersection> Triangle::intersect(Ray ray, float min_t) const {
  auto maybe = intersectUVWT(ray);
  if (!maybe)
    return {};
  glm::vec4 uvwt = maybe.value();

  // TODO: probably possible to do this earlier
  if (uvwt.w > min_t)
    return {};

  auto points = this->points();
  glm::vec3 e1 = points[1] - points[0];
  glm::vec3 e2 = points[2] - points[0];
  glm::vec3 n = glm::normalize(glm::cross(e1, e2));
  if (glm::dot(n, ray.dir) > 0) {
    n *= -1;
  }

  return Intersection{
      .ray = ray,
      .t = uvwt[3],
      .uv = glm::vec2(uvwt),
      .n2o = glm::mat3(e1, e2, n),
      .tri = *this,
  };
}

AABB Triangle::aabb() const {
  auto points = this->points();
  glm::vec3 min = points[0];
  glm::vec3 max = points[0];

  for (auto x : points) {
    min = glm::min(min, x);
    max = glm::max(max, x);
  }
  float min_size = 0.0f;
  glm::vec3 eps = glm::vec3(min_size);
  min -= eps;
  max += eps;

  glm::vec3 size = max - min;

  return AABB{.pos = min, .size = size};
}

std::shared_ptr<Transform> Triangle::transform() const {
  return obj->transform;
}

std::shared_ptr<Material> Triangle::material() const { return obj->material; }

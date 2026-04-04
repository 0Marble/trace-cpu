#include "Geometry.h"
#include "Log.h"
#include "Transform.h"
#include <cmath>
#include <limits>
#include <utility>

bool AABB::contains(glm::vec3 point) const {
  bool x = point.x >= pos.x && point.x < pos.x + size.x;
  bool y = point.y >= pos.y && point.y < pos.y + size.y;
  bool z = point.z >= pos.z && point.z < pos.z + size.z;
  return x && y && z;
}

AABB AABB::combine(AABB other) const {
  glm::vec3 a = glm::min(pos, other.pos);
  glm::vec3 b = glm::max(pos + size, other.pos + other.size);
  return {.pos = a, .size = b - a};
}

std::array<glm::vec3, 8> AABB::corners() const {
  return {
      pos + glm::vec3{0, 0, 0},           pos + glm::vec3{size.x, 0, 0},
      pos + glm::vec3{0, size.y, 0},      pos + glm::vec3{0, 0, size.z},
      pos + glm::vec3{size.x, size.y, 0}, pos + glm::vec3{size.x, 0, size.z},
      pos + glm::vec3{0, size.y, size.z}, pos + size,
  };
}

template <> AABB InstantTransform::apply<AABB>(AABB &aabb) {
  glm::mat4 mat = asMat();

  auto corners = aabb.corners();
  glm::vec3 min = glm::vec3(std::numeric_limits<float>::infinity());
  glm::vec3 max = glm::vec3(-std::numeric_limits<float>::infinity());
  for (auto p : corners) {
    glm::vec3 q = mat * glm::vec4(p, 1.0f);
    min = glm::min(q, min);
    max = glm::max(q, max);
  }

  return AABB{.pos = min, .size = max - min};
}

Geometry::~Geometry() {}

bool AABB::intersects(Ray ray) const {
  float t_min = 0.0f;
  float t_max = std::numeric_limits<float>::infinity();

  for (size_t i = 0; i < 3; i++) {
    float inv_ray_dir = 1.0f / ray.dir[i];
    float t_near = (pos[i] - ray.origin[i]) * inv_ray_dir;
    float t_far = (pos[i] + size[i] - ray.origin[i]) * inv_ray_dir;

    if (t_near > t_far) {
      std::swap(t_near, t_far);
    }

    t_min = glm::max(t_near, t_min);
    t_max = glm::min(t_far, t_max);

    if (t_min > t_max)
      return false;
  }

  return true;
}

std::ostream &operator<<(std::ostream &out, Geometry::Type t) {
  switch (t) {
  case Geometry::Type::Sphere:
    out << "Sphere";
    break;
  case Geometry::Type::Triangle:
    out << "Triangle";
    break;
  }
  return out;
}

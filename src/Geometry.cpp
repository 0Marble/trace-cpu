#include "Geometry.h"
#include "Transform.h"

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
  glm::vec3 a = aabb.pos;
  glm::vec3 b = aabb.pos + aabb.size;

  for (auto p : aabb.corners()) {
    glm::vec4 t = mat * glm::vec4{p.x, p.y, p.z, 1.0};
    p = glm::vec3{t.x, t.y, t.z};
    a = glm::min(a, p);
    b = glm::max(b, p);
  }

  return {.pos = a, .size = b - a};
}

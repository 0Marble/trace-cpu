#pragma once

#include "Ray.h"
#include <array>
#include <glm/glm.hpp>
#include <optional>

class Collision {
public:
  Ray ray;
  float t = 0.0f;
  glm::vec2 uv = {};
  glm::vec3 normal = {0.0f, 1.0f, 0.0f};
  glm::vec3 tangent = {0.0f, 0.0f, 1.0f};
  glm::vec3 pos = {};
};

class AABB {
public:
  glm::vec3 pos = {}, size = {1.0f, 1.0f, 1.0f};

  AABB combine(AABB other) const;
  bool contains(glm::vec3 point) const;
  std::array<glm::vec3, 8> corners() const;
};

class Geometry {
public:
  virtual ~Geometry();
  virtual std::optional<Collision> intersect(Ray ray) = 0;
  virtual AABB aabb() = 0;
};

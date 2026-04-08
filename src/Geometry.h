#pragma once

#include "Ray.h"
#include <array>
#include <glm/glm.hpp>
#include <optional>
#include <ostream>

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
  glm::vec3 pos = {}, size = {};

  AABB combine(AABB other) const;
  bool contains(glm::vec3 point) const;
  std::array<glm::vec3, 8> corners() const;

  bool intersects(Ray ray) const;
};

class Geometry {
public:
  enum class Type { Sphere, Triangle };

  virtual ~Geometry();
  virtual std::optional<Collision> intersect(Ray ray) = 0;
  virtual AABB aabb() = 0;
  virtual Type type() const = 0;
};

std::ostream &operator<<(std::ostream &out, Geometry::Type t);

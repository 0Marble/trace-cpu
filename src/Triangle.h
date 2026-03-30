#pragma once

#include "Geometry.h"
#include <array>
#include <glm/glm.hpp>

class Triangle : public Geometry {
public:
  std::array<glm::vec3, 3> points;

  AABB aabb() override;
  std::optional<Collision> intersect(Ray ray) override;
};

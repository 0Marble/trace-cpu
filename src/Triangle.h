#pragma once

#include "Geometry.h"
#include <array>
#include <glm/glm.hpp>

class Triangle : public Geometry {
  std::optional<glm::vec4> intersectUVWT(Ray ray);

public:
  std::array<glm::vec3, 3> points;

  AABB aabb() override;
  std::optional<Collision> intersect(Ray ray) override;
};

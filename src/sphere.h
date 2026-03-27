#pragma once

#include "geometry.h"

class Sphere : public Geometry {
public:
  float radius = 1.0f;

  AABB aabb() override;
  std::optional<Collision> intersect(Ray ray) override;
};

#pragma once

#include "geometry.h"

class Sphere : public Geometry {
public:
  AABB aabb() override;
  std::optional<Collision> intersect(Ray ray) override;
};

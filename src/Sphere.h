#pragma once

#include "Geometry.h"

class Sphere : public Geometry {
public:
  AABB aabb() override;
  std::optional<Collision> intersect(Ray ray, float min_t) override;
  Type type() const override;

private:
  std::optional<float> intersectT(Ray ray);
};

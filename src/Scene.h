
#pragma once

#include "Geometry.h"
#include "Object.h"
#include "Ray.h"
#include <vector>

class Scene {
  // TODO: use a BVH
  std::vector<Object> objects;

public:
  using Intersection = std::pair<Collision, Object *>;

  std::optional<Intersection> intersect(Ray ray);
};

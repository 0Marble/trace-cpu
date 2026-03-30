
#pragma once

#include "Geometry.h"
#include "Light.h"
#include "Object.h"
#include "Ray.h"
#include <vector>

class Scene {
  // TODO: use a BVH
  std::vector<Object> objects;

public:
  // TODO: any better ideas?
  std::vector<std::shared_ptr<Light>> lights;

  using Intersection = std::pair<Collision, Object *>;

  void addObject(Object);
  void addLight(std::shared_ptr<Light>);

  std::optional<Intersection> intersect(Ray ray);
};

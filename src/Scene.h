
#pragma once

#ifdef USE_BVH
#include "BVH.h"
#endif

#include "Geometry.h"
#include "Light.h"
#include "Object.h"
#include "Ray.h"
#include <vector>

class Scene {
#ifdef USE_BVH
  BVH bvh;
#endif
  std::vector<Object> objects;

public:
  // TODO: any better ideas?
  std::vector<std::shared_ptr<Light>> lights;

  using Intersection = std::pair<Collision, Object *>;

  void addObject(Object);
  void addLight(std::shared_ptr<Light>);
  void build(float start_time, float end_time);

  std::optional<Intersection> intersect(Ray ray);
};

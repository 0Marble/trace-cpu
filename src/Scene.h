
#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include <unordered_map>
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

  // TODO: I seem to recall that std::unordered_map is slow
  using MatCache = std::unordered_map<const Transform *, glm::mat4>;
#ifdef PARALLEL
  std::vector<MatCache> cache;
#else
  MatCache cache;
#endif

public:
  Scene();
  // TODO: any better ideas?
  std::vector<std::shared_ptr<Light>> lights;

  using Intersection = std::pair<Collision, Object *>;

  void addObject(Object);
  void addLight(std::shared_ptr<Light>);
  void build(float start_time, float end_time);

  // call at the start of every Raytracer::trace()
  void clearMatCache();
  std::optional<Intersection> intersect(Ray ray);
};

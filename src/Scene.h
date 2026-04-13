
#pragma once

#include "BVH.h"
#include "Camera.h"
#include "Intersection.h"
#include "Light.h"
#include "Material.h"
#include "Object.h"
#include "Ray.h"
#include "Triangle.h"
#include <memory>
#include <vector>

class Scene {
#ifdef USE_BVH
  BVH bvh = BVH();
#else
  std::vector<Object> objects = {};
#endif

public:
  std::vector<std::shared_ptr<Light>> lights = {};
  Camera camera;
  std::shared_ptr<Material> default_material;

  Scene(Camera cam);
  void addObject(Object object);
  void addLight(std::shared_ptr<Light> light);
  void startFrame(float start_time, float end_time);
  void clearCache();
  std::optional<Intersection> intersect(Ray ray);
};

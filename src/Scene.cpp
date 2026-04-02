#include "Scene.h"
#include "Light.h"
#include "glm/geometric.hpp"
#include <memory>

#include "Log.h"

void Scene::addObject(Object obj) { objects.push_back(obj); }

void Scene::addLight(std::shared_ptr<Light> light) { lights.push_back(light); }

void Scene::build(float start_time, float end_time) {
#ifdef USE_BVH
  bvh = BVH(objects, start_time, end_time);
  LOG(LogLevel::LOG_DEBUG, bvh);
#else
  (void)start_time;
  (void)end_time;
#endif
}

std::optional<Scene::Intersection> Scene::intersect(Ray ray) {

  ray.dir = glm::normalize(ray.dir);
  std::optional<Scene::Intersection> res = {};
  float dist = 0.0f;

#ifdef USE_BVH
  auto objs = bvh.potentialIntersections(ray);
  for (auto obj : objs) {
#else
  for (auto &obj_ref : objects) {
    Object *obj = &obj_ref;
#endif
    auto transform = obj->transform->sample(ray.time);
    Ray r = transform.applyInv(ray);

    auto col = obj->geometry->intersect(r);
    if (col && (!res || dist >= col->t)) {
      res = std::make_pair(col.value(), obj);
      dist = col->t;
    }
  }

  return res;
}

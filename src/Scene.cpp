#include "Scene.h"
#include "Light.h"
#include "glm/geometric.hpp"
#include <memory>

#ifdef PARALLEL
#include <omp.h>
#endif

Scene::Scene() : objects(), lights() {
#ifdef PARALLEL
  cache = std::vector<MatCache>(omp_get_max_threads());
#else
  cache = {};
#endif
}

void Scene::addObject(Object obj) { objects.push_back(obj); }

void Scene::addLight(std::shared_ptr<Light> light) { lights.push_back(light); }

void Scene::build(float start_time, float end_time) {
#ifdef USE_BVH
  bvh = BVH(objects, start_time, end_time);
#else
  (void)start_time;
  (void)end_time;
#endif
}

void Scene::clearMatCache() {
#ifdef PARALLEL
  int thread = omp_get_thread_num();
  MatCache &cache = this->cache[thread];
  cache.clear();
#else
  cache.clear();
#endif
}

std::optional<Scene::Intersection> Scene::intersect(Ray ray) {

  ray.dir = glm::normalize(ray.dir);
  std::optional<Scene::Intersection> res = {};
  float dist = 0.0f;

#ifdef PARALLEL
  int thread = omp_get_thread_num();
  MatCache &cache = this->cache[thread];
#else
  MatCache &cache = this->cache;
#endif

#ifdef USE_BVH
  static thread_local std::vector<Object *> objs;
  bvh.potentialIntersections(ray, objs);
  for (auto obj : objs) {
#else
  for (auto &obj_ref : objects) {
    Object *obj = &obj_ref;
#endif

    auto find = cache.find(obj->transform.get());
    glm::mat4 mat = {};
    if (find != cache.end()) {
      mat = find->second;
    } else {
      auto transform = obj->transform->sample(ray.time);
      mat = transform.asInv();
      cache[obj->transform.get()] = mat;
    }

    Ray r = {
        .origin = mat * glm::vec4(ray.origin, 1.0f),
        .dir = mat * glm::vec4(ray.dir, 0.0f),
        .time = ray.time,
    };

    auto col = obj->geometry->intersect(r);
    if (col && (!res || dist >= col->t)) {
      res = std::make_pair(col.value(), obj);
      dist = col->t;
    }
  }

  return res;
}

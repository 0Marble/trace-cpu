#include "Scene.h"
#include "DiffuseMaterial.h"
#include "Light.h"
#include "Log.h"
#include "Mesh.h"
#include "Transform.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/geometric.hpp"
#include <limits>
#include <memory>
#include <string_view>
#include <unordered_map>

#ifdef PARALLEL
#include <omp.h>
#endif

void Scene::addObject(Object obj) {
#ifdef USE_BVH
  bvh.addObject(obj);
#else
  objects.push_back(obj);
#endif
}

void Scene::addLight(std::shared_ptr<Light> light) { lights.push_back(light); }

void Scene::startFrame(float start_time, float end_time) {
#ifdef USE_BVH
  bvh.rebuild(start_time, end_time);
#else
  (void)start_time;
  (void)end_time;
#endif
}

using Cache = std::unordered_map<const Transform *, glm::mat4>;
static thread_local Cache cache;

void Scene::clearCache() { cache.clear(); }

std::optional<Intersection> Scene::intersect(Ray ray) {

  ray.dir = glm::normalize(ray.dir);
  std::optional<Intersection> res = {};
  float dist = std::numeric_limits<float>::infinity();

  static thread_local std::vector<Triangle> tris;
#ifdef USE_BVH
  bvh.potentialIntersections(ray, tris);
  for (auto tri : tris) {
#else
  for (auto &obj_ref : objects) {
    obj_ref.triangles(tris);
    for (auto tri : tris) {
#endif

    Transform *transform = tri.transform().get();
    auto find = cache.find(transform);
    glm::mat4 mat = {};
    if (find != cache.end()) {
      mat = find->second;
    } else {
      auto it = transform->sample(ray.time);
      mat = it.asInv();
      cache[transform] = mat;
    }

    Ray r = {
        .origin = mat * glm::vec4(ray.origin, 1.0f),
        .dir = mat * glm::vec4(ray.dir, 0.0f),
        .time = ray.time,
    };

    auto col = tri.intersect(r, dist);
    if (col) {
      res = col.value();
      dist = col->t;
    }
  }
#ifndef USE_BVH
}
#endif

return res;
}

Scene::Scene(Camera cam)
    : camera(cam), default_material(std::make_shared<DiffuseMaterial>(
                       glm::vec3(1.0f, 0.1f, 0.1f))) {}

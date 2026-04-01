#include "Scene.h"
#include "Light.h"
#include "glm/geometric.hpp"
#include <memory>

void Scene::addObject(Object obj) { objects.push_back(obj); }

void Scene::addLight(std::shared_ptr<Light> light) { lights.push_back(light); }

std::optional<Scene::Intersection> Scene::intersect(Ray ray) {
  ray.dir = glm::normalize(ray.dir);
  std::optional<Scene::Intersection> res = {};
  float dist = 0.0f;

  for (auto &obj : objects) {
    auto transform = obj.transform->sample(ray.time);
    Ray r = transform.applyInv(ray);

    auto col = obj.geometry->intersect(r);
    if (col && (!res || dist >= col->t)) {
      res = std::make_pair(col.value(), &obj);
      dist = col->t;
    }
  }

  return res;
}

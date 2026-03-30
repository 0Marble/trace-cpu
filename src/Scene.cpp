#include "Scene.h"
#include "Light.h"
#include <memory>

void Scene::addObject(Object obj) { objects.push_back(obj); }

void Scene::addLight(std::shared_ptr<Light> light) { lights.push_back(light); }

std::optional<Scene::Intersection> Scene::intersect(Ray ray) {
  for (auto &obj : objects) {
    auto transform = obj.transform->sample(ray.time);
    Ray r = transform.applyInv(ray);

    auto col = obj.geometry->intersect(r);
    if (col) {
      return {{col.value(), &obj}};
    }
  }

  return {};
}

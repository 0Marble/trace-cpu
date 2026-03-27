#include "Scene.h"

std::optional<Scene::Intersection> Scene::intersect(Ray ray) {
  for (auto &obj : objects) {
    auto transform = obj.transform->sample(ray.time);
    Ray r = transform.apply(ray);

    auto col = obj.geometry->intersect(r);
    if (col) {
      return {{col.value(), &obj}};
    }
  }

  return {};
}

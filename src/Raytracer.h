
#pragma once

#include "Random.h"
#include "Scene.h"
#include <memory>

class Raytracer {
public:
  std::shared_ptr<Scene> scene;
  std::shared_ptr<Random> rng;

  virtual glm::vec3 trace(Ray ray);
};

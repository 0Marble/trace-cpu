
#pragma once

#include "Random.h"
#include "Scene.h"
#include <memory>

class Raytracer {
public:
  std::shared_ptr<Scene> scene;
  std::shared_ptr<Random> rng;

  static const std::size_t bounce_cnt = 3;

  virtual glm::vec3 trace(Ray ray);

private:
  glm::vec3 traceRec(Ray ray, std::size_t bounce);
};

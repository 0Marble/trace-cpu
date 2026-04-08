
#pragma once

#include "Scene.h"
#include <memory>

class Raytracer {
public:
  std::shared_ptr<Scene> scene;
  size_t bounce_cnt = 3;
  float russian_roulette_prob = 0.6;

  Raytracer(std::shared_ptr<Scene> scene, size_t bounce_cnt = 3,
            float russian_roulette_prob = 0.6);

  virtual glm::vec3 trace(Ray ray);

private:
  glm::vec3 traceRec(Ray ray, std::size_t bounce);
};

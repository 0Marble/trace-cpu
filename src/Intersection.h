#pragma once

#include "Ray.h"
#include "Triangle.h"
#include "glm/ext/matrix_float3x3.hpp"
#include <glm/glm.hpp>

struct Intersection {
  Ray ray;
  float t;
  glm::vec2 uv;
  glm::mat3 n2o;
  Triangle tri;
};

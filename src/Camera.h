#pragma once

#include "Ray.h"
#include "Transform.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/gtc/constants.hpp"
#include <glm/glm.hpp>
#include <memory.h>
#include <memory>

struct Projection {
  float aspect = 1.0f;
  float fov = glm::half_pi<float>();
  float near = 0.1f;
  float far = 100.0f;
};

class Camera {
  glm::mat4 proj;
  std::shared_ptr<Transform> cam;

public:
  Camera(std::shared_ptr<Transform> transform, Projection proj = {});

  Ray shootRay(glm::vec2 uv, float time) const;
};

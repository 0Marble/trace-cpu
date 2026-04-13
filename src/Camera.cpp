#include "Camera.h"
#include "glm/ext/matrix_clip_space.hpp"

Camera::Camera(std::shared_ptr<Transform> transform, Projection proj)
    : proj(glm::perspective(proj.fov, proj.aspect, proj.near, proj.far)),
      cam(transform) {}

Ray Camera::shootRay(glm::vec2 uv, float time) const {
  InstantTransform transform = cam->sample(time);
  Ray ray = {.dir = glm::vec3(uv, 1), .time = time};
  ray.origin = proj * glm::vec4(ray.origin, 1.0f);
  ray.dir = proj * glm::vec4(ray.dir, 0.0f);
  ray = transform.apply(ray);
  ray.dir = glm::normalize(ray.dir);
  return ray;
}

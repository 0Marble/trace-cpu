#include "Transform.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"

glm::mat4 InstantTransform::asMat() const {
  glm::mat4 res = glm::scale(glm::mat4(1.0f), scale);
  res = glm::rotate(res, glm::angle(rotation), glm::axis(rotation));
  res = glm::translate(res, translation);
  return res;
}

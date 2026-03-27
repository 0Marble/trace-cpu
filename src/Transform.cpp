#include "Transform.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"

glm::mat4 InstantTransform::asMat() const {
  glm::mat4 res = glm::scale(glm::mat4(1.0f), scale);
  res = glm::rotate(res, glm::angle(rotation), glm::axis(rotation));
  res = glm::translate(res, translation);
  return res;
}

glm::mat4 InstantTransform::asInv() const { return inverse().asMat(); }

InstantTransform InstantTransform::inverse() const {
  return {
      .translation = -translation,
      .scale = 1.0f / scale,
      .rotation = glm::inverse(rotation),
  };
}

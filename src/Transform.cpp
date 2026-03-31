#include "Transform.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"

InstantTransform::InstantTransform() {}

InstantTransform::InstantTransform(glm::vec3 translation, glm::vec3 scale,
                                   glm::quat rotation, bool trs)
    : trs(trs), translation(translation), scale(scale), rotation(rotation) {}

glm::mat4 InstantTransform::asMat() const {
  glm::mat4 eye(1.0f);

  if (trs) {
    glm::mat4 res = glm::scale(eye, scale);
    res = glm::rotate(eye, glm::angle(rotation), glm::axis(rotation)) * res;
    res = glm::translate(eye, translation) * res;
    return res;
  } else {
    glm::mat4 res = glm::translate(eye, translation);
    res = glm::rotate(eye, glm::angle(rotation), glm::axis(rotation)) * res;
    res = glm::scale(eye, scale) * res;
    return res;
  }
}

glm::mat4 InstantTransform::asInv() const { return inverse().asMat(); }

InstantTransform InstantTransform::inverse() const {
  return {
      -translation,
      1.0f / scale,
      glm::inverse(rotation),
      !trs,
  };
}

InstantTransform InstantTransform::sample(float) { return *this; }

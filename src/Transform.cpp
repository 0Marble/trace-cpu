#include "Transform.h"
#include "Log.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/quaternion.hpp"

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

InstantTransform InstantTransform::lookAt(glm::vec3 pos, glm::vec3 at,
                                          glm::vec3 up) {
  glm::quat rot = glm::quatLookAt(glm::normalize(at - pos), glm::normalize(up));
  return InstantTransform(pos, glm::vec3(1), rot);
}

InstantTransform InstantTransform::lerp(InstantTransform other, float t) const {
  ASSERT(trs == other.trs);

  return InstantTransform((1.0f - t) * translation + t * other.translation,
                          (1.0f - t) * scale + t * other.scale,
                          glm::slerp(rotation, other.rotation, t), trs);
}

KeyframeTransform::KeyframeTransform(
    const std::vector<InstantTransform> &keyframes, float duration)
    : keyframes(keyframes), duration(duration) {}

InstantTransform KeyframeTransform::sample(float time) {
  float step = duration / (float)(keyframes.size() - 1);
  size_t prev = std::floor(time / step);
  size_t next = std::ceil(time / step);
  float t = time - std::floor(time / step);
  prev = std::min(keyframes.size() - 1, prev);
  next = std::min(keyframes.size() - 1, next);

  if (prev == next) {
    return keyframes[prev];
  }

  return keyframes[prev].lerp(keyframes[next], t);
}

OrbitTransform::OrbitTransform(glm::vec3 center, glm::vec3 start,
                               glm::vec3 axis, float rot_per_sec)
    : center(center), start(start), axis(axis),
      angular_velocity(rot_per_sec * glm::two_pi<float>()) {}

InstantTransform OrbitTransform::sample(float time) {
  float t = angular_velocity * time;
  glm::mat4 rot = glm::rotate(glm::mat4(1.0f), t, axis);
  glm::vec3 pos = rot * glm::vec4(start - center, 1.0f);
  return InstantTransform::lookAt(pos + center, center, axis);
}

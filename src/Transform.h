#pragma once

#include "AABB.h"
#include <glm/ext.hpp>
#include <glm/glm.hpp>

class InstantTransform;
class Transform {
public:
  virtual InstantTransform sample(float time) = 0;
  virtual AABB totalAABB(AABB aabb, float start_time, float end_time) = 0;

  virtual ~Transform() = default;
};

class InstantTransform : public Transform {
public:
  bool trs = true;
  glm::vec3 translation = {0, 0, 0};
  glm::vec3 scale = {1, 1, 1};
  glm::quat rotation = {1, 0, 0, 0};

  InstantTransform();
  InstantTransform(glm::vec3 translation, glm::vec3 scale = glm::vec3(1),
                   glm::quat rotation = {1, 0, 0, 0}, bool trs = true);

  static InstantTransform lookAt(glm::vec3 pos, glm::vec3 at, glm::vec3 up);

  template <class T> T apply(T &object);
  template <class T> T applyInv(T &object) { return inverse().apply(object); }

  glm::mat4 asMat() const;
  glm::mat4 asInv() const;
  InstantTransform inverse() const;

  InstantTransform lerp(InstantTransform other, float t) const;

  InstantTransform sample(float time) override;

  AABB totalAABB(AABB start, float start_time, float end_time) override;
};

class KeyframeTransform : public Transform {
public:
  std::vector<InstantTransform> keyframes;
  float duration;

  KeyframeTransform(const std::vector<InstantTransform> &keyframes,
                    float duration = 1.0f);

  InstantTransform sample(float time) override;

  AABB totalAABB(AABB start, float start_time, float end_time) override;
};

class OrbitTransform : public Transform {
public:
  glm::vec3 center;
  glm::vec3 start;
  glm::vec3 axis;
  float angular_velocity;

  OrbitTransform(glm::vec3 center, glm::vec3 start, glm::vec3 axis,
                 float rot_per_sec);

  InstantTransform sample(float time) override;

  AABB totalAABB(AABB start, float start_time, float end_time) override;
};

#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>

class InstantTransform {
public:
  glm::vec3 translation;
  glm::vec3 scale;
  glm::quat rotation;

  template <class T> T apply(T &object);

  glm::mat4 asMat() const;
  InstantTransform inverse() const;
};

class Transform {
public:
  virtual InstantTransform sample(float time) = 0;
};

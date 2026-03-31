#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <memory>

class InstantTransform;
class Transform {
public:
  virtual InstantTransform sample(float time) = 0;
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

  template <class T> T apply(T &object);
  template <class T> T applyInv(T &object) { return inverse().apply(object); }

  glm::mat4 asMat() const;
  glm::mat4 asInv() const;
  InstantTransform inverse() const;

  InstantTransform sample(float time) override;
};

class NestedTransform : public Transform {
public:
  std::shared_ptr<Transform> parent;
  std::shared_ptr<Transform> child;

  virtual InstantTransform sample(float time) = 0;
};

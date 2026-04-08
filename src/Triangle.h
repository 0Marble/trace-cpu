#pragma once

#include "Geometry.h"
#include "glm/fwd.hpp"
#include <array>
#include <glm/glm.hpp>

class Triangle : public Geometry {
public:
  std::array<glm::vec3, 3> points;

  Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c);

  AABB aabb() override;
  std::optional<Collision> intersect(Ray ray, float min_t) override;
  Type type() const override;

private:
  std::optional<glm::vec4> intersectUVWT(Ray ray);
};

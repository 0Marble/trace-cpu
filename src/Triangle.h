#pragma once

#include "AABB.h"
#include "Object.h"
#include "Ray.h"
#include "Transform.h"
#include <glm/glm.hpp>
#include <memory>
#include <optional>

struct Intersection;

class Triangle {
  const Object *obj;
  size_t face;

public:
  Triangle(const Object *obj, size_t face);

  AABB aabb() const;
  std::optional<Intersection> intersect(Ray ray, float min_t) const;
  std::shared_ptr<Transform> transform() const;
  std::shared_ptr<Material> material() const;
  std::array<glm::vec3, 3> points() const;

private:
  std::optional<glm::vec4> intersectUVWT(Ray ray) const;
};

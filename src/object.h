#pragma once

#include "geometry.h"
#include "material.h"
#include "transform.h"
#include <memory>

class Object {
public:
  std::shared_ptr<Geometry> geometry;
  std::shared_ptr<Material> material;
  std::shared_ptr<Transform> transform;
};

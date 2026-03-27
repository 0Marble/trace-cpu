#pragma once

#include "Geometry.h"
#include "Material.h"
#include "Transform.h"
#include <memory>

class Object {
public:
  std::shared_ptr<Geometry> geometry;
  std::shared_ptr<Material> material;
  std::shared_ptr<Transform> transform;
};

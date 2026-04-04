#pragma once

#include "Geometry.h"
#include "Material.h"
#include "Transform.h"
#include <memory>
#include <optional>
#include <string_view>

class Object {
public:
  std::shared_ptr<Geometry> geometry;
  std::shared_ptr<Material> material;
  std::shared_ptr<Transform> transform;
  std::optional<std::string_view> debug_name = {};

  friend std::ostream &operator<<(std::ostream &out, const Object &obj);
};

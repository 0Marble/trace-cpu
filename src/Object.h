#pragma once

#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include <memory>
#include <optional>
#include <string_view>

class Triangle;

class Object {
public:
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Material> material;
  std::shared_ptr<Transform> transform;
  std::optional<std::string_view> debug_name = {};

  void triangles(std::vector<Triangle> &tris) const;
};

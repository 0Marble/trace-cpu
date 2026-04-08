#pragma once

#include "../vendor/tiny_obj_loader.h"
#include "Geometry.h"
#include "Object.h"
#include <filesystem>
#include <memory>
#include <string>

class ObjMesh;
class ObjModel {
  tinyobj::ObjReader reader;
  std::filesystem::path file_path;
  friend ObjMesh;

public:
  ObjModel(const std::filesystem::path &file_path);

  static Object toObject(std::shared_ptr<ObjModel> obj, const std::string &name,
                         std::shared_ptr<Transform> transform);
};

class ObjMesh : public Geometry {
  std::shared_ptr<ObjModel> obj;
  const tinyobj::mesh_t *mesh;
  AABB cached_aabb;

  friend ObjModel;
  ObjMesh(std::shared_ptr<ObjModel> obj, const tinyobj::mesh_t *mesh);

public:
  AABB aabb() override;
  std::optional<Collision> intersect(Ray ray, float min_t) override;
  Type type() const override;
};

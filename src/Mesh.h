#pragma once

#include "../vendor/tiny_obj_loader.h"
#include <filesystem>
#include <memory>
#include <string_view>

class Mesh {
public:
  std::shared_ptr<tinyobj::ObjReader> file;
  std::string_view name;
  const tinyobj::mesh_t *mesh;

  Mesh(std::shared_ptr<tinyobj::ObjReader> file, std::string_view name);
  Mesh(std::filesystem::path obj_path, std::string_view name);
};

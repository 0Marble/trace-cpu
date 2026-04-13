#include "Mesh.h"
#include "Log.h"

Mesh::Mesh(std::shared_ptr<tinyobj::ObjReader> file, std::string_view name)
    : file(file), name(name), mesh(nullptr) {
  for (auto &shape : file->GetShapes()) {
    if (shape.name == name) {
      mesh = &shape.mesh;
      break;
    }
  }
  ASSERT(mesh, "file", name, "not found");
}

Mesh::Mesh(std::filesystem::path obj_path, std::string_view name) {
  auto reader = std::make_shared<tinyobj::ObjReader>();
  tinyobj::ObjReaderConfig cfg;
  cfg.mtl_search_path = obj_path.parent_path();
  bool ok = reader->ParseFromFile(obj_path, cfg);
  ASSERT(ok, reader->Error());

  Mesh(reader, name);
}

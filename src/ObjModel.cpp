#include "ObjModel.h"
#include "DiffuseMaterial.h"
#include "Geometry.h"
#include "Log.h"
#include "Triangle.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <filesystem>
#include <limits>
#include <memory>

ObjModel::ObjModel(const std::filesystem::path &file_path)
    : reader(), file_path(file_path) {
  auto parent_dir = file_path.parent_path();

  tinyobj::ObjReaderConfig cfg{};
  cfg.mtl_search_path = parent_dir;
  bool success = reader.ParseFromFile(file_path, cfg);
  ASSERT(success, "couldn't load model", file_path, ":", reader.Error());
}

Object ObjModel::toObject(std::shared_ptr<ObjModel> self,
                          const std::string &name,
                          std::shared_ptr<Transform> transform) {

  const tinyobj::mesh_t *mesh = nullptr;
  for (auto &shape : self->reader.GetShapes()) {
    if (shape.name == name) {
      mesh = &shape.mesh;
      break;
    }
  }

  ASSERT(mesh, "object '", name, "' not found in file", self->file_path);
  ObjMesh m(self, mesh);

  return Object{
      .geometry = std::make_shared<ObjMesh>(m),
      .material =
          std::make_shared<DiffuseMaterial>(glm::vec3(1.0f, 0.1f, 0.1f)),
      .transform = transform,
  };
}

ObjMesh::ObjMesh(std::shared_ptr<ObjModel> obj, const tinyobj::mesh_t *mesh)
    : obj(obj), mesh(mesh) {

  glm::vec3 min(std::numeric_limits<float>::infinity());
  glm::vec3 max(-std::numeric_limits<float>::infinity());

  const auto &verts = obj->reader.GetAttrib().GetVertices();
  for (size_t face_idx = 0; face_idx < mesh->num_face_vertices.size();
       face_idx++) {
    ASSERT(mesh->num_face_vertices[face_idx] == 3,
           "only triangle faces are supported");

    auto a = mesh->indices[3 * face_idx + 0].vertex_index;
    auto b = mesh->indices[3 * face_idx + 1].vertex_index;
    auto c = mesh->indices[3 * face_idx + 2].vertex_index;

    auto tri =
        Triangle(glm::make_vec3(&verts[3 * a]), glm::make_vec3(&verts[3 * b]),
                 glm::make_vec3(&verts[3 * c]));

    for (auto p : tri.points) {
      min = glm::min(min, p);
      max = glm::max(max, p);
    }
  }

  cached_aabb = {.pos = min, .size = max - min};
}

AABB ObjMesh::aabb() { return cached_aabb; }

std::optional<Collision> ObjMesh::intersect(Ray ray, float min_t) {
  const auto &verts = obj->reader.GetAttrib().GetVertices();

  std::optional<Collision> res = {};

  // TODO: this is quite slow, ideally there exists a object-local bvh, or we
  // somehow hook into the global one
  for (size_t face_idx = 0; face_idx < mesh->num_face_vertices.size();
       face_idx++) {

    auto a = mesh->indices[3 * face_idx + 0].vertex_index;
    auto b = mesh->indices[3 * face_idx + 1].vertex_index;
    auto c = mesh->indices[3 * face_idx + 2].vertex_index;

    auto tri =
        Triangle(glm::make_vec3(&verts[3 * a]), glm::make_vec3(&verts[3 * b]),
                 glm::make_vec3(&verts[3 * c]));

    auto col = tri.intersect(ray, min_t);
    if (!col) {
      continue;
    }

    min_t = col->t;
    res = col;
  }

  return res;
}

Geometry::Type ObjMesh::type() const { return Type::Obj; }

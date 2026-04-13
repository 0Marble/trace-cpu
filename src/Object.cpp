#include "Object.h"
#include "Triangle.h"

void Object::triangles(std::vector<Triangle> &tris) const {
  tris.clear();
  for (size_t face = 0; face < mesh->mesh->num_face_vertices.size(); face++) {
    tris.emplace_back(this, face);
  }
}

#pragma once

#include "AABB.h"
#include "Object.h"
#include "Triangle.h"
#include <ostream>
#include <vector>

class BVH {
  struct Node {
    bool is_leaf;
    std::vector<Triangle> tris = {};
    size_t left = 0;
    size_t right = 0;
    AABB aabb = AABB();
  };

  std::vector<Object> objects = {};
  std::vector<Node> nodes = {};
  size_t root = 0;

public:
  BVH() = default;

  void addObject(Object obj);
  void rebuild(float start_time, float end_time);
  void potentialIntersections(Ray ray, std::vector<Triangle> &tris);

  friend std::ostream &operator<<(std::ostream &out, const BVH &bvh);

private:
  size_t construct(std::vector<Triangle> &&tris, float start_time,
                   float end_time);
  void dump(std::ostream &out, const Node *node, size_t depth = 0) const;
};

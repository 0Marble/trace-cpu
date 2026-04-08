#pragma once

#include "Geometry.h"
#include "Object.h"
#include "Ray.h"
#include <ostream>
#include <vector>

class BVH {
  struct Node {
    bool is_leaf;
    size_t left = 0;
    size_t right = 0;
    AABB aabb = {};
    std::vector<Object *> objects = {};
  };

  std::vector<Object> objects = {};
  std::vector<Node> nodes = {};
  size_t root;
  float start_time = 0;
  float end_time = 0;

public:
  BVH(BVH &&self) = default;
  BVH() = default;
  BVH(const BVH &) = default;
  BVH(const std::vector<Object> &objects, float start_time, float end_time);

  BVH &operator=(BVH &&other) = default;

  void potentialIntersections(Ray ray, std::vector<Object *> &out);

  friend std::ostream &operator<<(std::ostream &out, const BVH &bvh);

private:
  size_t construct(std::vector<Object *> &&objects);

  void dump(std::ostream &out, const Node *node, size_t depth = 0) const;
};

#pragma once

#include "Geometry.h"
#include "Object.h"
#include "Ray.h"
#include <memory>
#include <ostream>
#include <variant>
#include <vector>

class BVH {
  struct LeafNode;
  struct InnerNode;
  struct DummyNode {};
  using Node = std::variant<std::shared_ptr<LeafNode>,
                            std::shared_ptr<InnerNode>, DummyNode>;

  struct LeafNode {
    std::vector<Object *> objects = {};
    AABB aabb = {};
  };

  struct InnerNode {
    Node left;
    Node right;
    AABB aabb;
  };

  std::vector<Object> objects = {};
  float start_time = 0;
  float end_time = 0;

  Node root = DummyNode{};

public:
  BVH(BVH &&self) = default;
  BVH() = default;
  BVH(const BVH &) = default;
  BVH(const std::vector<Object> &objects, float start_time, float end_time);

  BVH &operator=(BVH &&other) = default;

  std::vector<Object *> potentialIntersections(Ray ray);

  friend std::ostream &operator<<(std::ostream &out, const BVH &bvh);

private:
  std::vector<Object *> potentialIntersections(Ray ray, Node n);
  Node construct(std::vector<Object *> &&objects);

  void dump(std::ostream &out, Node node, size_t depth = 0) const;
};

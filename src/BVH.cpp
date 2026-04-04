#include "BVH.h"
#include "Log.h"
#include "Object.h"
#include "VecFmt.h"
#include "glm/ext/vector_float3.hpp"
#include <cstddef>
#include <memory>
#include <vector>

BVH::BVH(const std::vector<Object> &objects, float start_time, float end_time)
    : objects(objects), start_time(start_time), end_time(end_time) {

  std::vector<Object *> refs(objects.size());
  for (size_t i = 0; i < objects.size(); i++) {
    refs[i] = &this->objects[i];
  }

  root = construct(std::move(refs));
}

std::vector<Object *> BVH::potentialIntersections(Ray ray) {
  return potentialIntersections(ray, root);
}
std::vector<Object *> BVH::potentialIntersections(Ray ray, Node n) {
  switch (n.index()) {
  case 0: {
    auto leaf = std::get<0>(n);

    if (leaf->aabb.intersects(ray)) {
      return leaf->objects;
    } else {
      return {};
    }

  } break;
  case 1: {
    auto node = std::get<1>(n);
    if (!node->aabb.intersects(ray)) {
      return {};
    }

    auto l = potentialIntersections(ray, node->left);
    auto r = potentialIntersections(ray, node->right);
    for (auto x : r) {
      l.push_back(x);
    }
    return l;
  } break;
  default:
    UNREACHABLE("handled all valid cases");
  }
}

size_t absdiff(size_t a, size_t b) {
  if (a > b)
    return a - b;
  return b - a;
}

// TODO: this is pretty inefficient, but it only runs once at the start
BVH::Node BVH::construct(std::vector<Object *> &&objects) {
  size_t n = objects.size();
  if (n == 0) {
    return std::make_shared<LeafNode>(LeafNode{});
  }

  AABB total = {};
  std::vector<glm::vec3> mins(objects.size());
  std::vector<glm::vec3> maxs(objects.size());

  for (size_t i = 0; i < n; i++) {
    Object *o = objects[i];
    AABB obj_aabb =
        o->transform->totalAABB(o->geometry->aabb(), start_time, end_time);
    if (i == 0) {
      total = obj_aabb;
    } else {
      total = total.combine(obj_aabb);
    }
    mins[i] = obj_aabb.pos;
    maxs[i] = obj_aabb.pos + obj_aabb.size;
  }

  size_t best_diff = objects.size();
  std::vector<Object *> best_left{};
  std::vector<Object *> best_right{};

  for (size_t dim = 0; dim < 3; dim++) {
    for (size_t i = 0; i < n; i++) {
      float c_min = mins[i][dim];
      float c_max = maxs[i][dim];

      std::vector<Object *> left;
      std::vector<Object *> right;
      std::vector<Object *> mid;

      bool clean_split = true;

      for (size_t j = 0; j < n; j++) {
        float min = mins[j][dim];
        float max = maxs[j][dim];

        if (max <= c_min) {
          left.push_back(objects[j]);
        } else if (min >= c_max) {
          right.push_back(objects[j]);
        } else if (max <= c_max && min >= c_min) {
          mid.push_back(objects[j]);
        } else {
          clean_split = false;
          break;
        }
      }

      if (!clean_split) {
        continue;
      }

      size_t d1 = absdiff(left.size() + mid.size(), right.size());
      size_t d2 = absdiff(left.size(), mid.size() + right.size());

      if (d1 < best_diff) {
        best_diff = d1;
        best_left = left;
        best_right = right;
        for (auto x : mid) {
          best_left.push_back(x);
        }
      } else if (d2 < best_diff) {
        best_diff = d2;
        best_left = left;
        best_right = right;
        for (auto x : mid) {
          best_right.push_back(x);
        }
      }
    }
  }

  if (best_diff == objects.size()) {
    return std::make_shared<LeafNode>(
        LeafNode{.objects = objects, .aabb = total});
  } else {
    return std::make_shared<InnerNode>(InnerNode{
        .left = construct(std::move(best_left)),
        .right = construct(std::move(best_right)),
        .aabb = total,
    });
  }
}

void BVH::dump(std::ostream &out, Node node, size_t depth) const {
  std::string space(depth * 2, ' ');
  switch (node.index()) {
  case 0: {
    auto l = std::get<0>(node);
    out << space << "- " << VecFmt(l->aabb.pos) << "--"
        << VecFmt(l->aabb.pos + l->aabb.size) << "\n";
    for (auto o : l->objects) {
      out << space << "- " << *o << "\n";
    }
  } break;
  case 1: {
    auto n = std::get<1>(node);
    out << space << "- " << VecFmt(n->aabb.pos) << "--"
        << VecFmt(n->aabb.pos + n->aabb.size) << "\n";
    out << space << "- left:\n";
    dump(out, n->left, depth + 1);
    out << space << "- right:\n";
    dump(out, n->right, depth + 1);
  } break;
  default:
    UNREACHABLE("handled all valid cases");
  }
}

std::ostream &operator<<(std::ostream &out, const BVH &bvh) {
  out << "- root:\n";
  bvh.dump(out, bvh.root, 1);
  return out;
}

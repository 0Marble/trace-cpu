#include "BVH.h"
#include "Object.h"
#include "VecFmt.h"
#include "glm/ext/vector_float3.hpp"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <utility>
#include <vector>

BVH::BVH(const std::vector<Object> &objects, float start_time, float end_time)
    : objects(objects), nodes(), root(0), start_time(start_time),
      end_time(end_time) {

  std::vector<Object *> refs(objects.size());
  for (size_t i = 0; i < objects.size(); i++) {
    refs[i] = &this->objects[i];
  }

  root = construct(std::move(refs));
}

void BVH::potentialIntersections(Ray ray, std::vector<Object *> &out) {
  out.clear();
  if (nodes.size() == 0) {
    return;
  }

  static thread_local std::vector<Node *> stack = {};
  stack.clear();
  stack.push_back(&nodes[root]);

  while (!stack.empty()) {
    Node *n = stack.back();
    stack.pop_back();

    if (n->aabb.intersects(ray)) {
      if (n->is_leaf) {
        std::copy(n->objects.begin(), n->objects.end(),
                  std::back_inserter(out));
      } else {
        stack.push_back(&nodes[n->left]);
        stack.push_back(&nodes[n->right]);
      }
    }
  }

  return;
}

size_t absdiff(size_t a, size_t b) {
  if (a > b)
    return a - b;
  return b - a;
}

// TODO: this is pretty inefficient, but it only runs once at the start
size_t BVH::construct(std::vector<Object *> &&objects) {
  size_t n = objects.size();
  if (n == 0) {
    size_t idx = nodes.size();
    nodes.push_back(Node{.is_leaf = true});
    return idx;
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
    size_t idx = nodes.size();
    nodes.push_back(Node{
        .is_leaf = true,
        .aabb = total,
        .objects = objects,
    });
    return idx;
  } else {
    size_t left = construct(std::move(best_left));
    size_t right = construct(std::move(best_right));
    size_t idx = nodes.size();
    nodes.push_back(Node{
        .is_leaf = false,
        .left = left,
        .right = right,
        .aabb = total,
    });
    return idx;
  }
}

void BVH::dump(std::ostream &out, const Node *node, size_t depth) const {
  std::string space(depth * 2, ' ');
  if (node->is_leaf) {
    out << space << "- " << VecFmt(node->aabb.pos) << "--"
        << VecFmt(node->aabb.pos + node->aabb.size) << "\n";
    for (auto o : node->objects) {
      out << space << "- " << *o << "\n";
    }
  } else {
    out << space << "- " << VecFmt(node->aabb.pos) << "--"
        << VecFmt(node->aabb.pos + node->aabb.size) << "\n";
    out << space << "- left:\n";
    dump(out, &nodes[node->left], depth + 1);
    out << space << "- right:\n";
    dump(out, &nodes[node->right], depth + 1);
  }
}

std::ostream &operator<<(std::ostream &out, const BVH &bvh) {
  out << "- root:\n";
  bvh.dump(out, &bvh.nodes[bvh.root], 1);
  return out;
}

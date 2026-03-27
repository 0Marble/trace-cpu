#include "Raytracer.h"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include <array>

struct Step {
  Collision col;
  Object *obj;
  glm::mat4 w2o;
  glm::mat4 o2w;
  glm::mat3 o2n;
  glm::mat3 n2o;
};

glm::vec3 Raytracer::trace(Ray ray) {
  static const std::size_t bounce_cnt = 10;

  std::array<Step, bounce_cnt> history = {};
  std::size_t cnt = 0;

  for (; cnt < bounce_cnt; cnt++) {
    auto collision = scene->intersect(ray);
    if (!collision) {
      break;
    }

    auto [col, obj] = collision.value();
    InstantTransform transform = obj->transform->sample(ray.time);
    glm::mat3 to_normal =
        glm::mat3(col.normal, col.tangent, glm::cross(col.normal, col.tangent));
    Step step = {
        .col = col,
        .obj = obj,
        .w2o = transform.asMat(),
        .o2w = transform.asInv(),
        .o2n = to_normal,
        .n2o = glm::transpose(to_normal),
    };

    glm::vec3 incoming = step.o2n * (step.w2o * glm::vec4(ray.dir, 0.0f));
    glm::vec3 outgoing =
        obj->material->sampleReflectedDir(*rng, incoming, col.uv, ray.time);

    ray.origin = step.o2w * glm::vec4(col.pos, 1.0f);
    ray.dir = step.o2w * glm::vec4(step.n2o * outgoing, 0.0f);

    history[cnt] = step;
  }

  glm::vec3 color = {};
  for (std::size_t j = 0; j < cnt; j++) {
    std::size_t i = cnt - j - 1;
    Step step = history[i];

    glm::vec3 to_view = {};
    glm::vec3 to_light = {};
    if (j == 0) {
      to_light = ray.dir;
    } else {
      to_light = history[i + 1].col.pos - step.col.pos;
    }
    to_light = step.o2n * (step.w2o * glm::vec4(to_light, 0.0f));

    // TODO: assuming camera at (0,0,0)
    if (i == 0) {
      to_view = -step.col.pos;
    } else {
      to_view = history[i - 1].col.pos - step.col.pos;
    }
    to_view = step.o2n * (step.w2o * glm::vec4(to_view, 0.0f));

    color = step.obj->material->sampleColor(*rng, to_light, to_view, color,
                                            step.col.uv, ray.time);
  }

  return color;
}

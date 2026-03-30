#include "Raytracer.h"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include <array>

struct Step {
  Collision col;
  Object *obj;
  glm::vec3 light;
  glm::mat4 w2o;
  glm::mat4 o2w;
  glm::mat3 o2n;
  glm::mat3 n2o;
  glm::vec3 to_view_n;
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
    glm::mat3 normal =
        glm::mat3(col.tangent, glm::cross(col.normal, col.tangent), col.normal);
    Step step = {
        .col = col,
        .obj = obj,
        .light = glm::vec3(0),
        .w2o = transform.asInv(),
        .o2w = transform.asMat(),
        .o2n = glm::transpose(normal),
        .n2o = normal,
        .to_view_n = glm::vec3(0),
    };

    if (cnt == 0) {
      // NOTE:
      // using ray.origin as camera pos because its the first intersection
      step.to_view_n = glm::vec3(step.w2o * glm::vec4(ray.origin, 1)) - col.pos;
    } else {
      glm::vec4 prev_pos_world =
          history[cnt - 1].o2w * glm::vec4(history[cnt - 1].col.pos, 1);
      glm::vec4 prev_pos_obj = step.w2o * prev_pos_world;
      step.to_view_n = glm::vec3(prev_pos_obj) - col.pos;
    }

    glm::vec3 incoming = step.o2n * (step.w2o * glm::vec4(ray.dir, 0.0f));
    glm::vec3 outgoing =
        obj->material->sampleReflectedDir(*rng, incoming, col.uv, ray.time);

    ray.origin = step.o2w * glm::vec4(col.pos, 1.0f);

    for (auto &light : scene->lights) {
      glm::vec3 light_pos_w = light->worldPos(ray.time);
      ray.dir = glm::normalize(light_pos_w - ray.origin);
      if (!scene->intersect(ray)) {
        auto light_color = light->sample(ray.dir, ray.time);
        if (!light_color)
          continue;

        glm::vec3 light_dir = step.o2n * (step.w2o * glm::vec4(ray.dir, 0));

        // TODO: attenuation? same as below
        step.light +=
            obj->material->sampleColor(*rng, light_dir, step.to_view_n,
                                       light_color.value(), col.uv, ray.time);
      }
    }

    ray.dir = glm::normalize(step.o2w * glm::vec4(step.n2o * outgoing, 0.0f));

    history[cnt] = step;
  }

  glm::vec3 color = {0, 0, 0};
  for (std::size_t j = 0; j < cnt; j++) {
    std::size_t i = cnt - j - 1;
    Step step = history[i];

    glm::vec3 to_light = {};
    if (i + 1 == cnt) {
      to_light = step.w2o * glm::vec4(ray.dir, 0);
    } else {
      glm::vec4 next_pos_world =
          history[i + 1].o2w * glm::vec4(history[i + 1].col.pos, 1);
      glm::vec4 next_pos_obj = step.w2o * next_pos_world;
      to_light = glm::vec3(next_pos_obj) - step.col.pos;
    }
    to_light = step.o2n * to_light;

    // TODO: light attenuation? how it depends on the distance. Same as above
    color = step.obj->material->sampleColor(*rng, to_light, step.to_view_n,
                                            color, step.col.uv, ray.time) +
            step.light;
  }

  return color;
}

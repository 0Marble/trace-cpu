#include "Raytracer.h"
#include "Log.h"
#include "Random.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/random.hpp"
#include "glm/matrix.hpp"
#include <cstdlib>
#include <vector>

Raytracer::Raytracer(std::shared_ptr<Scene> scene, size_t bounce_cnt,
                     float russian_roulette_prob)
    : scene(scene), bounce_cnt(bounce_cnt),
      russian_roulette_prob(russian_roulette_prob) {}

struct Step {
  Ray incoming;
  glm::vec3 direct_light = glm::vec3(0);
  glm::vec3 indirect_light = glm::vec3(0);
  size_t bounce = 0;
  glm::vec3 weight = glm::vec3(0);
  size_t secondary_ray_cnt = 0;
  size_t prev = 0;
  glm::vec3 color = glm::vec3(0);
};

glm::vec3 Raytracer::trace(Ray from_camera_ray) {
  std::vector<Step> steps{bounce_cnt + 1};
  size_t read = 0;
  size_t write = 1;
  float continue_prob = russian_roulette_prob;

  steps[0] = Step{.incoming = from_camera_ray};

  while (read != write) {
    Step &s = steps[read];
    read++;

    auto collision = scene->intersect(s.incoming);
    if (!collision) {
      continue;
    }

    auto [col, obj] = collision.value();
    InstantTransform transform = obj->transform->sample(s.incoming.time);
    auto n2o =
        glm::mat3(col.tangent, glm::cross(col.normal, col.tangent), col.normal);
    auto w2o = transform.asInv();
    auto o2w = transform.asMat();
    auto o2n = glm::transpose(n2o);

    Ray secondary_ray = s.incoming;
    glm::vec3 to_view_n = glm::normalize(
        o2n * (glm::vec3(w2o * glm::vec4(secondary_ray.dir, 0))));
    secondary_ray.origin = o2w * glm::vec4(col.pos, 1.0f);

    s.direct_light = glm::vec3(0);
    for (auto &light : scene->lights) {
      Ray shadow_ray = secondary_ray;
      glm::vec3 light_pos_w = light->worldPos(shadow_ray.time);
      shadow_ray.dir = glm::normalize(light_pos_w - shadow_ray.origin);

      if (scene->intersect(shadow_ray).has_value()) {
        continue;
      }

      auto light_color = light->sample(shadow_ray.dir, shadow_ray.time);
      if (!light_color)
        continue;

      glm::vec3 to_light_n =
          glm::normalize(o2n * (w2o * glm::vec4(shadow_ray.dir, 0)));

      s.direct_light +=
          obj->material->bsdf(col.uv, shadow_ray.time, to_view_n, to_light_n) *
          light_color.value() * std::max(to_light_n.z, 0.0f) /
          glm::dot(light_pos_w - shadow_ray.origin,
                   light_pos_w - shadow_ray.origin);
    }

    if (Random::uniform() > continue_prob || s.bounce >= bounce_cnt) {
      continue;
    }

    float pdf = 0.0f;
    glm::vec3 bsdf = glm::vec3(0.0f);
    glm::vec3 to_obj_n =
        obj->material->sample(col.uv, secondary_ray.time, to_view_n, pdf, bsdf);
    secondary_ray.dir = glm::normalize(o2w * glm::vec4(n2o * to_obj_n, 0.0f));

    Step next = {
        .incoming = secondary_ray,
        .bounce = s.bounce + 1,
        .weight = bsdf / pdf * std::max(to_obj_n.z, 0.0f) / continue_prob,
        .prev = read - 1,
    };

    ASSERT(write < steps.size());
    steps[write++] = next;
  }

  for (size_t j = 0; j < write; j++) {
    size_t i = write - j - 1;

    ASSERT(i < steps.size());
    Step &cur = steps[i];
    if (cur.secondary_ray_cnt == 0) {
      cur.color = cur.direct_light;
    } else {
      cur.color = cur.direct_light +
                  (cur.indirect_light) / (float)cur.secondary_ray_cnt;
    }
    if (cur.bounce == 0) {
      break;
    }

    ASSERT(cur.prev < steps.size());
    Step &prev = steps[cur.prev];
    prev.secondary_ray_cnt++;
    prev.indirect_light += cur.color * cur.weight;
  }

  return steps[0].color;
}
